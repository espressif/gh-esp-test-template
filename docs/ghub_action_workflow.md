# ESP-IDF Project Template with CI and Hardware Testing Workflow

Here you will find a brief description of how to configure and customize the build and test workflow.

## Project Repository Layout

This repository follows the project layout as described in detail in [Build System](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/build-system.html) section of ESP-IDF Programming Guide.

**See as example layout of this repository:**
1. Application code is in the [components](../components/) and [main](../main) directories (example of a real application)
  ```sh
  ├── /.github
  ├── /components    # <---  (Components of user app)
  ├── /main          # <---  (Main component of user app)
  ├── /test_app      # <---  (Test application)
  └── CMakeLists.txt # <---  (User app CMakeLists.txt)
  ```
  You can have multiple apps in a GitHub repository and add them all to the testing process by specifying their paths in the `test_app/CMakeLists.txt` file (see the following text).

2. In the file ` test_app/CMakeLists.txt` specify the path to your testable components (components that contain unit tests):
  ```sh
  ├── components
  ├── /test_app
  │   ├── CMakeLists.txt   # <---  (define paths for the build system)

  ```
  ```cmake
  ...
      # Specify the path to testable components
      set(EXTRA_COMPONENT_DIRS ../components)  # <--- (these components will be built and tested)
  ...
  ```

## GitHub Action Workflow File (`ci_workflow.yml`)

This is the workflow file that will control the whole process.

In this file, you will define the workflow triggers, jobs, and dependencies.

The workflow with build and tests is triggered in three ways:

- Schedule - automatically once per day at midnight
- Pull Requests
- Manually from GitHub Action UI

GitHub Action workflow file `.github/workflows/ci_workflow.yml`:

```yml
name: Build and Run Test Application Workflow

on:
  schedule:
    - cron: '0 0 * * *' # (scheduled - once per day at midnight)
  pull_request: # (for every push to Pull Request)
    types: [opened, reopened, synchronize]
  workflow_dispatch:  # (to start the workflow manually)

jobs:
  build_project:
    name: Build Project
    uses: ./.github/workflows/build_esp_app.yml
# Disable this job if you don't use it for hardware testing
  test_project:
    if: ${{ github.repository_owner == 'espressif' }}  # Disable this job as default except if the repo is owned by Espressif
    name: Test Project
    uses: ./.github/workflows/test_esp_app.yml
    needs: build_project
```

### Jobs

In this file, you will see two `jobs` defined as follows:

#### Job build_project

This job will call the build workflow, defined by the file `.github/workflows/build_esp_app.yml`.

#### Job test_project

This job will call the test workflow, defined by the file `.github/workflows/test_esp_app.yml`.

As default, this workflow is disabled if the namespace is different from `espressif`. To enable, please set the `if: ${{ github.repository_owner == 'espressif' }}` to your namespace or remove this line.

```yml
  test_project:
    if: ${{ github.repository_owner == 'espressif' }}  # Disable this job as default except if the repo is owned by Espressif
    name: Test Project
    uses: ./.github/workflows/test_esp_app.yml
    needs: build_project
    ...
```

## Project Build Workflow (`build_esp_app.yml`)

### Build Binaries (`build` Job)

First, the binaries must be built with respect to each Espressif chip (`esp-idf target`). Using the GitHub action [matrix strategy](https://docs.github.com/en/actions/using-jobs/using-a-matrix-for-your-jobs), you can build for several targets (chip types) in parallel.

Building application binaries directly on a test runner (Raspberry Pi) would be very ineffective due to its limited computing power, so the build job runs on a significantly more powerful GitHub-hosted runner.

The build workflow will take care of building the project for a single or multiple targets.

You can define the target list in the `espidf_target`.

```yml
  ...
  build:
    name: Build Test App
    runs-on: ubuntu-20.04
    strategy:
      fail-fast: false
      matrix:
        espidf_target: ["esp32", "esp32c3", "esp32s2", "esp32s3"]  # <--- Specify a list of Espressif chips connected to your runner
    ...
```

You can customize the build configuration per your requirements.

The step for building the code can be also customized. This job is based on the [esp-idf-ci-action](https://github.com/espressif/esp-idf-ci-action).

Here you can customize the ESI-IDF version, the target (see the previous step), and the path to the application.

```yml
  ...
      - name: Build Test Application with ESP-IDF
        uses: espressif/esp-idf-ci-action@v1
        with:
          esp_idf_version: "latest" # latest is the default version
          target: ${{ matrix.espidf_target }}
          path: 'test_app'
    ...
```

To set the ESP-IDF version, you can use the release tag name in the `esp_idf_version` field.

**Example**

```yml
  ...
          esp_idf_version: "v5.0"
    ...
```

To see the full release tag list, please see the [project release list](https://github.com/espressif/esp-idf/tags).

## Test on Hardware Workflow (`test_esp_app.yml`)

Binaries built in the `build` job are downloaded via GH Action artifact and used in this job,
which is running on your self-hosted runner with connected Espressif boards.

- binaries are flashed to boards
- tests are performed
- results are gathered for the final report

GitHub needs to know which of your self-hosted runner it should use for the test. It is a good practice to tag self-hosted runners in the same format as targets in `esp-idf`.

For example, the runner tag `esp32c3` is also the `IDF_TARGET` target. By defining this value in [matrix](https://docs.github.com/en/actions/using-jobs/using-a-matrix-for-your-jobs), the same value can be used as a `IDF_TARGET` for the `build` job and as a runner tag for the `run-target` job.

### Tests in Docker Containers

We recommend running all tests in Docker containers because
  - you always start with a clean and identical test environment
  - there is no need to clean up after testing
  - no risk of permanent unwanted system changes on runner machines

The Docker container must have **access to the computer's USB ports** - set the `--privileged` option in the job container definition:

Since the testing framework uses Python (`pytest`), it is a good idea to have a base image for a container with Python already included, e.g. `python:3.7-buster` (based on Debian, with Python 3).

```yml
  run-target:
    name: Run Test App on target
    needs: build
    runs-on: [self-hosted, linux, docker, "${{ matrix.espidf_target }}"]
    container:
      image: python:3.7-buster
      options: --privileged  # <--- Privileged mode has access to serial ports
    ...
```

#### Publish results (`test_esp_app.yml`)

Test results are published directly to the repository action in form of a nice table.

[Publish Unit Test Result Action](https://github.com/EnricoMi/publish-unit-test-result-action) is used for parsing test results into GitHub test summaries.

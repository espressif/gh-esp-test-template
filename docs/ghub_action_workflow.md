# Project repository layout and workflow implementation

## Tests in Docker containers

We recommend running all tests in Docker containers because 
  - you always start with a clean and identical test environment
  - there is no need to clean up after testing 
  - no risk of permanent unwanted system changes on runner machines


The Docker container must have **access to the computer's USB ports** - set the `--privileged` option in the job container definition:

Since the testing framework uses Python (`pytest`), it is a good idea to have a base image for a container with Python already included, e.g. `python:3.7-buster` (based on Debian, with Python 3).

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



## Project repository layout

This repository follows project layout as described in detail in [Build System](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/build-system.html) section of ESP-IDF Programming Guide.

**See as example layout of this repository:**
1. Application code is in the [components](../components/) and [main](../main) directories (example of a real application)
    ```sh
    ├── /.github
    ├── /components    # <---  (Components of user app)
    ├── /main          # <---  (Main component of user app)
    ├── /test_app
    └── CMakeLists.txt # <---  (User app CMakeLists.txt)
    ```
    You can have multiple apps in a GitHub repository and add them all to the testing process by specifying their paths in the `test_app/CMakeLists.txt` file (see the following text).
___
2. Your components can contain subdirectory `/test` with unit tests
    ```sh
    ├── testable_component
    │   ├── ...
    │   ├── test   # <---  (subdirectory with unit tests of testable_component)
    │   │   ├── ...
    ```
___
3. In the file ` test_app/CMakeLists.txt` specify path to your testable components (components that contain unit tests):
    ```sh
    ├── components
    ├── /test_app
    │   ├── CMakeLists.txt   # <---  (define paths for the build system)

    ```
    ```cmake
    ...
        # Specify path to testable components
        set(EXTRA_COMPONENT_DIRS ../components)  # <--- (these components will be built)
        set(TEST_COMPONENTS testable)            # <--- (these components will be tested)
    ...
    ```
___

## GitHub Action workflow (yaml file)

### Inputs
In the GitHub Action workflow file`.github/workflows/build_and_test_esp_app.yml` define:
```yml
jobs:
  build:
    name: Build Test App
    ...
    container: espressif/idf:latest  # <--- Specify version of ESP-IDF in image tag
    strategy:
      matrix:
        espidf_target: ["esp32", "esp32c3"]  # <--- Specify a list of Espressif chips connected to your runner
    ...

  run-target:
    name: Run Test App on target
    ...
    strategy:
      matrix:
        espidf_target: ["esp32", "esp32c3"]  # <--- Specify a list of Espressif chips connected to your runner
```

### Triggers
Workflow with tests in this example is triggered in three ways:
- Schedule - automatically once per day at midnight
- Pull requests
- Manually from GitHub Action UI

### Jobs
#### Build binaries (`build`)
First, the binaries must be built with respect to each Espressif chip (`esp-idf target`). Using the GitHub action [matrix strategy](https://docs.github.com/en/actions/using-jobs/using-a-matrix-for-your-jobs), you can build for several targets (chip types) in parallel.

Building application binaries directly on test runner (Raspberry Pi) would be very ineffective due to its limited computing power, so the build job runs on significantly more powerful GitHub-hosted runner.

___
#### Test on hardware (`run-target`)
Binaries built in the `build` job are downloaded via GH Action artifact and used in this job,
which is running on your self-hosted runner with connected Espressif boards. 

- binaries are flashed to boards
- tests are performed
- results are gathered for final report

GitHub needs to know which of your self-hosted runner it should use for the test. It is a good practice to tag self-hosted runners in the same format as targets in `esp-idf`.

For example, runner tag `esp32c3` is also `IDF_TARGET` target. By defining this in a [matrix](https://docs.github.com/en/actions/using-jobs/using-a-matrix-for-your-jobs) the same value can be used as target for `build` step a and as runner tag for `run-target` step.

@todo 'step' and 'job' is used here multiple times. Is it the same thing?

___
#### Publish results (`publish-results`)
Test results are published directly to the repository action in form of a nice table.

[Publish Unit Test Result Action](https://github.com/EnricoMi/publish-unit-test-result-action) is used for parsing test results into GitHub test summaries.

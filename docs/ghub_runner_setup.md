# GitHub Runner Setup

## Hardware
This example uses a small **Raspberry Pi** single board computer to install the GitHub runner agent and **connect the two Espressif development boards**.

  - Raspberry Pi 4 Model B - 2GB RAM
  - SD card Kingston Micro SDHC 32GB 100MB/s
  - Espressif development board `ESP32-DevKitc`
  - Espressif development board `ESP32-C3-DevKitM-1`
  - short shielded `USB-A <-> Micro USB` cables


## Install RaspiOS and software packages
- connect the boards via USB cables to the Raspberry Pi
- download and write to SD card image of the operating system (`Raspberry Pi OS Lite (Legacy) 32bit`)
- connect to Raspberry and install `git` and `docker`


## Install and setup GitHub runner
*To add a self-hosted GitHub runner to your GitHub organization account, you must be the owner of the organization.*

Read GitHub documentation about [Adding Self Hosted Runners](https://docs.github.com/en/actions/hosting-your-own-runners/adding-self-hosted-runners)


#### Install GitHub runner agent on the machine
Go to your GitHub account `Settings -> Actions -> Runners -> New Runner` and follow the instructions to install it on your Raspberry Pi:
- select Linux/ARM architecture *(for Raspberry PI)*
- install the GitHub runner agent under a user account, e.g. under the default user `pi` (the user must have sudo permissions)
- create the GitHub runner service available on reboot (`sudo ./svc.sh install`)
- reboot Pi and check, if the service was launched automatically (`sudo ./svc.sh status`)

- in your GitHub account `Settings -> Actions -> Runners` you should now see a new online GitHub runner

#### Add GitHub runner tags
Go to `Settings -> Actions -> Runner groups -> Default (or another group name)` and click on the name of
your new runner to **add tags**

- in our example, we use runner tags with the same name as the esp-idf targets - this allows us to use common names in the matrix strategy of the GitHub Action workflow:
```yml
...
run-target:
    name: Run Test App on target
    runs-on: [self-hosted, linux, docker, "${{ matrix.espidf_target }}"]  #<---(esp-idf targets are also runner tags)
    strategy:
      matrix:
        espidf_target: ["esp32", "esp32c3"]  #<---(esp-idf targets are also runner tags)
  ...
```
<br>

  ![GitHub Runner example tags](imgs/ghub_runner_tags_example.png)

  | Board              | TAG / esp-idf target |
  |--------------------|----------------------|
  | ESP32-DevKitc      | `esp32`              |
  | ESP32-C3-DevKitM-1 | `esp32c3`            |

- you can choose tags that describe the product and board revision, or anything else to suit your needs. In this case, you must specify custom runner tags in `run-target/runs-on:` instead of matrix-strategy.

#### Enable GitHub runner for repository
Go to `Settings -> Actions -> Runner groups -> Default -> Repository Access` -> select your project or allow this runner for all projects in the organization

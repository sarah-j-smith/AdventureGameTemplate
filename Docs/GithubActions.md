# Github Actions

[Github Actions is a continuous integration platform] built into the Github code hosting platform.

It can be used to make builds, and to run unit tests among other things.

At present the goal is to have it run some simple automation tests. The idea would be to have 
anyone else who wants to contribute to the project be able to have their code checked by 
a set of unit tests.

[Github Actions is a continuous integration platform]: https://docs.github.com/en/actions/get-started/quickstart

## Using the Github Hosted Unreal Docker images

This repo shows the container references in the `ghcr.io` repository for all different versions of
Unreal Engine. The next section shows how to use these container references:

* https://github.com/orgs/epicgames/packages/container/unreal-engine/492471794?tag=dev-5.6.1

Requires membership of the Epic Games Github organisation and acceptance of the Terms

## Running the Docker Image locally

* Install Podman or Docker
* Run the commands below:
  * The login command will ask for a password to a GitHub account 
  * That account must be a member of the Epic Games github

```
podman login myuser@domain.com ghcr.io

podman run --platform linux/amd64 --rm -ti \
    -v "/Users/sez/depot/AdventureTemplate:/project" \
    ghcr.io/epicgames/unreal-engine:dev-slim-5.6.1
```

* The `podman run` command will:
  * download the container
  * run it **_and_** 
  * bring up the command line

## Logging in to GHCR in Github Actions

Note that here I use a specifically created [Personal Access Token] (PAT) with access only to the packages, the PAT has 
no privileges to operate on any of the rest of my github. I add that to the secrets manager
in github, along with the username I access github with. By doing this I avoid the need to 
add the 

  `permissions: packages`

that is [required if using the GitHub supplied vars]

[required if using the GitHub supplied vars]: https://stackoverflow.com/a/78866108/813919

[Personal Access Token]: https://docs.github.com/en/authentication/keeping-your-account-and-data-secure/managing-your-personal-access-tokens

## How the Container is Mounted in Github Actions

Lots of additional volume mounts are used to give access to the external runner machine. The github
actions are run as the user `1001` in Linux and the user name is `runner`.

Generally making as few changes as possible and accepting defaults is the best way to get things
to work.

```shell
/usr/bin/docker create --name c7eed3bc2c424962a1eafad562212635_ghcrioepicgamesunrealenginedevslim561_a264f6 
    --label 89e2d2 --workdir /__w/AdventureGameTemplate/AdventureGameTemplate 
    --network github_network_c47a0789e0a047d4b9612e844b33f855 --user root -e "HOME=/github/home"
    -e GITHUB_ACTIONS=true -e CI=true -v "/var/run/docker.sock":"/var/run/docker.sock" 
    -v "/project" 
    -v "/home/runner/work":"/__w" -v "/home/runner/actions-runner/cached/2.336.0/externals":"/__e":ro 
    -v "/home/runner/work/_temp":"/__w/_temp" -v "/home/runner/work/_actions":"/__w/_actions" 
    -v "/opt/hostedtoolcache":"/__t" -v "/home/runner/work/_temp/_github_home":"/github/home" 
    -v "/home/runner/work/_temp/_github_workflow":"/github/workflow" 
    --entrypoint "tail" ghcr.io/epicgames/unreal-engine:dev-slim-5.6.1 "-f" "/dev/null"
```

## The Yaml File Steps

At present the file `unreal-unit-tests.yml` does the following:

* Pulls down and mounts the dev-slim-5.6.1 Unreal Engine container
* Checks out the AdventureTemplate source code in the workdir (see above shell command)
* Checks out the PaperZD plugin source code in `Plugins/PaperZD`
* If the PaperZD plugin has a line `EngineVersion` then it patches that line to be "5.6.0"
* Runs `./Scripts/Build.sh` and puts the builds in $PROJECT_DIR/Packaged
* Runs `./Scripts/Test.sh` 
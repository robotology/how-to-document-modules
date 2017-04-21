## Introduction

This template repository provides a detailed guide on how your modules, and possibly their API you exposed as _IDL services_, should be documented.

A clean and handy way to deal with your documentation is to exploit the [**GitHub Pages**](https://pages.github.com/), which represents a nice tool to publish software documentation online.

Everything is basically done through the special branch called **`gh-pages`**. The branch must contain at the root level the **index.html** file pointing to the static documentation (generally generated via [**doxygen**](http://www.doxygen.org)) stored somewhere within the branch itself. Further, the static documentation might be composed of many products (sometimes images), thus it would be worth saving space on the repository by not retaining any history for those files.

First off, we explain how to **create the infrastructure** for the documentation using _git_ and keep it **up-to-date** throughout the changes the code undergoes. Then, we will dig into how we can use **doxygen** to generate the documentation.

Let's start :sparkles:

### Static documentation (i.e. you have to build it manually!)

#### Creating the infrastructure
From `master`, create a new branch called `gh-pages` on your repository. Then, locally do:

```
git checkout master
git fetch origin
git branch gh-pages --track origin/gh-pages
git checkout gh-pages
```
Now you have the same branch `gh-pages` also on your machine which tracks the _remote_ `gh-pages`.

Next step is to create the **static documentation locally**:
- Provide thus a **sub-directory** called **`doxygen`** where you have to put the file **`generate.txt`** that tells doxygen how to produce the documentation.
You can find a template `generate.txt` file [here](https://github.com/robotology/how-to-document-modules/blob/gh-pages/doxygen/generate.txt) within this repository.

    Inside the `generate.txt` file, the most important parameter you are required to fill in is **`INPUT`**, which specifies the directories containing your code to be documented (a recursive search is typically done). An example follows:
    ```
    INPUT = ../src \
            ../idl_dox \
            generated-from-xml
    ```
    Typically, to be neat, the file `generate.txt` contains instructions to generate the documentation under **doxygen/doc** via the parameter **`OUTPUT_DIRECTORY`**.

    To find out more about other doxygen parameters, we suggest you to look directly at the online guide.

- Now type:
```
cd doxygen
doxygen ./generate.txt
 ```
 This will generate your documentation. The new directory **doxygen/doc** will appear containing all your documentation.

- Create the `index.html` file at the root level of your repository that provides the entry point to your documentation. This is an [**example**](https://github.com/robotology/how-to-document-modules/blob/gh-pages/index.html) you have to tailor to your specific repository.

- Stage, commit and push:
```
git add ./doxygen
git add ./index.html
git commit -m "provided doxygen documentation"
git push origin gh-pages
```

After publishing the changes, you can visit the page http://robotology.github.com/how-to-document-modules (of course use your github account and repository name in the url) and you will be redirected to the online documentation.

Finally, it is also a good practice to cite that url from within the `README.md` file.


#### Updating the documentation
By creation, the special `gh-pages` branch should always mirror the `master` branch and should contain two things more: the doxygen directory along with the `index.html` file. Regarding the commit history, `gh-pages` should be always [one commit ahead of `master`](https://github.com/robotology/how-to-document-modules/network).

Whenever you update `master` branch then, do the following to update the documentation accordingly:

```
git checkout gh-pages
git rebase master
cd doxygen
rm -rf ./doc
doxygen ./generate.txt
git add ./doc
git log -1
git commit --amend
git push --force-with-lease
git checkout master
```
The **`git log -1`** command serves as verification and does display the very last commit message on the `gh-pages` branch, which must be "*provided doxygen documentation*", that is the one specified initially at creation time. The combination of **`git commit --amend`** and **`git push --force-with-lease`** aim to modify the latest stored commit instead of creating a brand new one and eventually force publishing it. This way, we always retain only one commit for the documentation instead of dealing with its whole history.


### Automatic documentation (i.e. Let Travis do the job!)

#### Creating the infrastructure
In your git project folder create a new **empty** branch called `gh-pages` on your repository.

```
cd your-project-folder
git checkout --orphan gh-pages
```

If your project is not a newly created project you probably already have tracked files.
The following command will clean the working area.

```
git rm -rf .
```

Now create the *permanent* files (the ones which will not be overwritten by the automatic documentation procedure). Usually this means a `.gitignore` file, possibly different from the one in the `master` branch of your project, and the `index.html` file which will point to the documentation.
If your doxygen configuration generates the *html* docs in `html` folder, the following `index.html` should be ok.

```html
<!DOCTYPE HTML>
<html lang="en-US">
    <head>
        <meta charset="UTF-8">
        <meta http-equiv="refresh" content="1;url=html/index.html">
        <title>Page Redirection</title>
    </head>
    <body>
        If you are not redirected automatically, follow the <a href="html/index.html">link to the documentation</a>
    </body>
</html>
```

Now you can commit and push to the `gh-pages` branch
```
git add .gitignore index.html
git commit -m "initial commit"
git push origin gh-pages
```

Now we have to configure **Travis-CI** to automatically generate the documentation and push to the `gh-pages` branch at every commit. This can be done by modifying the `.travis.yml` file.
Open your `travis.yml` file, and in the `after_success` block add the following code

```yaml
[...]
after_success:
  # Generate the docs only if master, the travis_build_docs is true and we can use secure variables
  - >-
    if [[ "$TRAVIS_BRANCH" = "master" && -n "$TRAVIS_BUILD_DOCS" && "$TRAVIS_PULL_REQUEST" = "false" ]] ; then
      cd $PROJECT_DIR_ABS
      source .ci/setup-ssh.sh || travis_terminate 1
      .ci/generateDocumentation.sh || travis_terminate 1
    fi
```

Note that the `if` statement checks three conditions:
- the branch must be `master` (for now multiple versions docs are not supported)
- the user-defined variable `TRAVIS_BUILD_DOCS` must be defined. This is used to use only one travis job to build the documentation in case a build matrix is present
- builds on pull requests are disabled. Only normal commits on `master` are used. Note that once a PR is accepted, it always generates a commit on the destination branch

In the previous lines two additional scripts located in the `.ci` folder are called.
One, `.ci/setup-ssh.sh` is needed to manage authentication in Travis (as Travis needs to push on the repository the generated documentation). We discuss how to setup SSH for Travis in [Setup SSH use for Travis](#setup-ssh-use-for-travis).
The second script, `.ci/generateDocumentation.sh` is the script which actually calls doxygen. See [Generate the documentation](#generate-the-documentation) for details on the script.

#### Setup SSH use for Travis

There are multiple options to give Travis push permissions on the repository. We chose to use the GitHub deploy keys as they are not tied to a particular user but only to a single repository.

To add a new deploy key to your repository follow these [instructions](https://developer.github.com/guides/managing-deploy-keys/#deploy-keys).
Once we added the new key, we have to encrypt (to increase the security) and to give to Travis the private key.
The easiest way it to use the Travis Command line tools and follow [this guide](https://docs.travis-ci.com/user/encrypting-files). 
Follow the printed instructions. The command line utility should have already updated your repository settings in Travis, by adding the encrypted variables. You have to add to the repository **onky** the encrypted private key.

The script to load the ssh key in travis is the following

```sh
#!/bin/bash
# setup-ssh.sh: load the SSH key 

set -ev
declare -r SSH_FILE="$(mktemp -u $HOME/.ssh/travis_temp_ssh_key_XXXX)"
# Decrypt the file containing the private key (put the real name of the variables)
openssl aes-256-cbc \
  -K $encrypted_XXXXXXXXXX_key \
  -iv $encrypted_XXXXXXXXXX_iv \
  -in ".ci/travis_deploy_key.enc" \
  -out "$SSH_FILE" -d
# Enable SSH authentication
chmod 600 "$SSH_FILE" \
  && printf "%s\n" \
       "Host github.com" \
       "  IdentityFile $SSH_FILE" \
       "  LogLevel ERROR" >> ~/.ssh/config
```

#### Generate the documentation
The last step is to write the script to actually generate the documentation.
This is the full script:

```sh
#!/bin/sh
################################################################################
# Title         : generateDocumentation.sh
# Date created  : 2017
# Notes         : original script from Jeroen de Bruijn
__AUTHOR__="Francesco Romano"
# Preconditions:
# - Packages doxygen doxygen-doc doxygen-gui graphviz
#   must be installed.
# - Doxygen configuration file must have the destination directory empty and
#   source code directory with a $(TRAVIS_BUILD_DIR) prefix.
# - An gh-pages branch should already exist. See below for mor info on hoe to
#   create a gh-pages branch.
#
# Required global variables:
# - TRAVIS_BUILD_NUMBER : The number of the current build.
# - TRAVIS_COMMIT       : The commit that the current build is testing.
# - DOXYFILE            : The Doxygen configuration file.
# - GH_REPO_NAME        : The name of the repository.
# - GH_REPO_REF         : The GitHub reference to the repository.
# - GH_REPO_TOKEN       : Secure token to the github repository.
#
# For information on how to encrypt variables for Travis CI please go to
# https://docs.travis-ci.com/user/environment-variables/#Encrypted-Variables
# or https://gist.github.com/vidavidorra/7ed6166a46c537d3cbd2
# For information on how to create a clean gh-pages branch from the master
# branch, please go to https://gist.github.com/vidavidorra/846a2fc7dd51f4fe56a0
#
# This script will generate Doxygen documentation and push the documentation to
# the gh-pages branch of a repository specified by GH_REPO_REF.
# Before this script is used there should already be a gh-pages branch in the
# repository.
# 
################################################################################

################################################################################
##### Setup this script and get the current gh-pages branch.               #####
echo 'Setting up the script...'
# Exit with nonzero exit code if anything fails
set -e

GH_REPO_ORG=`echo $TRAVIS_REPO_SLUG | cut -d "/" -f 1`
GH_REPO_NAME=`echo $TRAVIS_REPO_SLUG | cut -d "/" -f 2`
GH_REPO_REF="github.com/$GH_REPO_ORG/$GH_REPO_NAME.git"
DOXYFILE=$TRAVIS_BUILD_DIR/doc/Doxyfile


# Get the current gh-pages branch
git clone -b gh-pages git@github.com:$GH_REPO_ORG/$GH_REPO_NAME.git code_docs
cd code_docs

# Copy DoxygenLayout.xml here
cp $TRAVIS_BUILD_DIR/doc/DoxygenLayout.xml .

##### Configure git.
# Set the push default to simple i.e. push only the current branch.
git config --global push.default simple
# Pretend to be an user called Travis CI.
git config user.name "Travis CI"
git config user.email "travis@travis-ci.org"

# go back to first commit
git reset --hard `git rev-list --max-parents=0 --abbrev-commit HEAD`

# Need to create a .nojekyll file to allow filenames starting with an underscore
# to be seen on the gh-pages site. Therefore creating an empty .nojekyll file.
# Presumably this is only needed when the SHORT_NAMES option in Doxygen is set
# to NO, which it is by default. So creating the file just in case.
echo "" > .nojekyll

################################################################################
##### Generate the Doxygen code documentation and log the output.          #####
echo 'Generating Doxygen code documentation...'
# Redirect both stderr and stdout to the log file AND the console.
doxygen $DOXYFILE 2>&1 | tee doxygen.log

################################################################################
##### Upload the documentation to the gh-pages branch of the repository.   #####
# Only upload if Doxygen successfully created the documentation.
# Check this by verifying that the html directory and the file html/index.html
# both exist. This is a good indication that Doxygen did it's work.
if [ -d "html" ] && [ -f "html/index.html" ]; then
    echo 'Uploading documentation to the gh-pages branch...'
    # Add everything in this directory (the Doxygen code documentation) to the
    # gh-pages branch.
    # GitHub is smart enough to know which files have changed and which files have
    # stayed the same and will only update the changed files.
    git add --all

    # Commit the added files with a title and description containing the Travis CI
    # build number and the GitHub commit reference that issued this build.
    git commit -m "Deploy code docs to GitHub Pages Travis build: ${TRAVIS_BUILD_NUMBER}" -m "Commit: ${TRAVIS_COMMIT}"

    # Force push to the remote gh-pages branch.
    # The ouput is redirected to /dev/null to hide any sensitive credential data
    # that might otherwise be exposed.
    git push --force "git@github.com:$GH_REPO_ORG/$GH_REPO_NAME.git" > /dev/null 2>&1
else
    echo '' >&2
    echo 'Warning: No documentation (html) files have been found!' >&2
    echo 'Warning: Not going to push the documentation to GitHub!' >&2
    exit 1
fi

```

Note that in the above script there are some assumptions:
- the main `.dox` file is defined by the `DOXYFILE` variable.
- If you use a Doxygen Layout file, we copy it in the current directory: `cp $TRAVIS_BUILD_DIR/doc/DoxygenLayout.xml .`. This is because it seems that the DoxygenLayout file path is taken with respect to where the `doxygen` command is executed.
- To keep the repository clean we remove all the commits but the first one. Note that in order to keep the script simple at least one commit should exist (and it does if you followed this guide)

### How to write the documentation

You have to provide the documentation of your module through an _xml file_:

- [**modulename.xml**](https://github.com/robotology/how-to-document-modules/blob/master/how-to-document-modules.xml) for the general description.

Moreover, if your module also exposes an IDL generated API, then the corresponding services should be documented in the relative _thrift file_:

- [**idl.thrift**](https://github.com/robotology/how-to-document-modules/blob/master/src/idl.thrift) listing and documenting all the available services.

We don't go here into more details since at the following links you'll find official guidelines on how to write the documentation for:
- [xml description of your module](http://wiki.icub.org/brain/xml_module_documentation.html).
- [Thrift services](http://www.yarp.it/thrift_tutorial_simple.html).

Of course, aside from the documentation generated automatically from the code, you should also fill the `README.md` file with information about **installation instructions**, **dependencies**, so as descriptions regarding the **goal of your module**, the **theory behind your implementation**, **tutorials** to make the learning curve of your software more gentle, and everything is necessary to help the user :smile:.

#### Documentation generated from xml
The xml file containing the general description of the modules cannot be automatically digested by doxygen, requiring first a conversion into the dox language by means of a style sheet parser shipped with Yarp. To this end, you can profitably rely on scripts we made for you, which automate this operation:
- [doc-compile.sh](https://github.com/robotology/how-to-document-modules/blob/gh-pages/doxygen/doc-compile.sh): a bash script for Linux.
- [doc-compile.ps1](https://github.com/robotology/how-to-document-modules/blob/gh-pages/doxygen/doc-compile.ps1): a powershell script for Windows.

Dependencies to run these scripts are:
- [**doxygen**](http://www.doxygen.org), of course :smile:
- [**xsltproc**](http://www.sagehill.net/docbookxsl/InstallingAProcessor.html), on Linux you can get it via `sudo apt-get install xsltproc`.
- **`YARP_ROOT`** environment variable set (but you can customize the scripts as you like)

These scripts need to be located within the doxygen directory. When run, they replace the `rm -rf ./doc` and `doxygen ./generate.txt` instructions in the flow above by applying style sheet conversion and then producing documentation in one go. The final products will be then put under the **doxygen/generated-from-xml** directory. Therefore, doxygen must be aware of the latter path when collecting all the files we want to document. For this reason, this path is declared in the **`INPUT`** section of the `generate.txt` file.

#### Why do we also specify the **idl_dox** directory as further input?
This directory is necessary only if you provide Thrift services using the `yarp_add_idl` cmake directive. In this case, the header file **modulename_IDL.h** (or any equivalent name according to your setting) will be generated automatically inside the **build**, which is generally ignored by doxygen in its search. Thereby, as **temporary workaround solution**, we suggest to `copy and paste` the header file from the build into the **idl_dox** stub and make doxygen aware of its presence by filling the **`INPUT`** parameter.

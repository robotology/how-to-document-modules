## Automatic documentation

### Creating the infrastructure

In your git project folder create a new **empty** branch called `gh-pages` on your repository.

```sh
$ cd your-project-folder
$ git checkout --orphan gh-pages
```

If your project is not a newly created project you probably already have tracked files.
The following command will clean the working area.

```sh
$ git rm -rf .
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

```sh
$ git add .gitignore index.html
$ git commit -m "initial commit"
$ git push origin gh-pages
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

### Setup SSH use for Travis

There are multiple options to give Travis push permissions on the repository. We chose to use the GitHub deploy keys as they are not tied to a particular user but only to a single repository.

To add a new deploy key to your repository follow these [instructions](https://developer.github.com/guides/managing-deploy-keys/#deploy-keys).
Once we added the new key, we have to encrypt (to increase the security) and to give to Travis the private key.
The easiest way it to use the Travis Command line tools and follow [this guide](https://docs.travis-ci.com/user/encrypting-files).
Follow the printed instructions. The command line utility should have already updated your repository settings in Travis, by adding the encrypted variables. You have to add to the repository **only** the encrypted private key.

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

### Generate the documentation

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
- If you use a Doxygen Layout file, we copy it in the current directory:
```sh
cp $TRAVIS_BUILD_DIR/doc/DoxygenLayout.xml .
```
This is because it seems that the DoxygenLayout file path is taken with respect to where the `doxygen` command is executed.
- To keep the repository clean we remove all the commits but the first one. Note that in order to keep the script simple at least one commit should exist (and it does if you followed this guide)

# how-to-document-modules
Scripts and templates to help you document your code nicely. 

In the following sections, you can find a detailed description on how to document your module and, if available, your thrift services. You can look at this repo structure and files as a model for your developing documented repo.

Please be careful: the implemented code is just a _fake module_ to show you how to deal with documentation!

## Description
A clean way to handle your module documentation is to exploit the [GitHub Pages](https://pages.github.com/), which can be used to publish software documentation online
. Everything is basically done through the special branch called `gh-pages`. The branch must contain at the root level the **index.html** file pointing to the static documentation stored somewhere within the branch itself. However, the documentation generated with tools like _doxygen_ might be composed of many products (sometimes images), thus it would be worth saving space on the repo by not retaining any history for those files.

First off, we explain how to **create the infrastructure** for the documentation using git and keep it **up-to-date** over changes the code undergoes. Then, we will go into the details on how to use **doxygen** to write up the documentation.

Let's start:

### Creating the infrastructure
From `master`, create a new branch called `gh-pages` on your GitHub repository. Then, locally do:

```
git checkout master
git fetch origin
git branch gh-pages --track origin/gh-pages
git checkout gh-pages
```
Now you have the same branch `gh-pages` also on your machine which tracks the _remote_ `gh-pages`.

Next step is to create the **static documentation locally**:
- Provide thus a **sub-folder** called **`doxygen`** where you have to put the file **`generate.txt`** that tells doxygen how to generate the documentation.
You can find a template for you `generate.txt` file [here](https://github.com/robotology-playground/how-to-document-modules/blob/gh-pages/doxygen/generate.txt).

    A lot of variables are available. The most important one is **`INPUT`** that specifies the folders containing your code to be documented (a recursive search is typically done). An example follows:
    ```
    INPUT = ../src \
            ../idl_dox \
            generated-from-xml
    ```
    Usually, to be neat, the file `generate.txt` contains instructions to put the generated documentation under **doxygen/doc** (this is recalled later) via the variable **`OUTPUT_DIRECTORY`**.

- Now type:
```
cd doxygen
doxygen ./generate.txt
 ```
 This will generate your documentation. A new folder, called **doc** will appear containing all your documentation.

- Stage, commit and push the doxygen directory so as the **`index.html`** file to be located at the root level.
```
git add ./doxygen
git add ./index.html
git commit -m "provided doxygen documentation"
git push origin gh-pages
```
- Your `index.html` file should look like [**this one**](https://github.com/robotology-playground/how-to-document-modules/blob/gh-pages/index.html), pointing to the generated html page, which you would like to be the home page of your documentation. After publishing the changes, you will have the url http://my-account.github.com/my-repository pointing to the documentation linked in the **index.html** on the web. In this case the url is http://robotology-playground.github.com/how-to-document-modules. It is advisable to cite that url from within the README.md file in this way for example: [how-to-document-modules documentation](http://robotology-playground.github.io/how-to-document-modules).


### Updating the documentation
By creation, the special `gh-pages` branch should always mirror the `master` branch and should contain two things more: the doxygen folder along with the `index.html` file. Regarding the commit history, `gh-pages` should be always [one commit ahead the `master`](https://github.com/robotology-playground/how-to-document-modules/network).

Whenever you update `master` branch then, do the following to update the documentation accordingly:

```
git checkout gh-pages
git rebase master
cd doxygen
rm -rf doc
doxygen ./generate.txt
git add ./doc
git log -1
git commit --amend
git push --force-with-lease
git checkout master
```
The **`git log -1`** command serves as verification and does display the very last commit message on the `gh-pages` branch, which must be "*provided doxygen documentation*", that is the one specified initially at creation time. The combination of **`git commit --amend`** and **`git push --force-with-lease`** aim to modify the latest stored commit instead of creating a brand new one and eventually force publishing it. This way, we always retain only one commit for the documentation instead of dealing with its whole history.

### How to write the documentation

The usage of two differen kinds of files is is strongly suggested for code documentation:

- **idl.thrift** file if your module includes a thrift service;
- **module-name.xml** for the general description of your module.

At the following links, you can find some guidelines on how to write the documentation for  
[thrift service](http://www.yarp.it/thrift_tutorial_simple.html) and [general description of your module](http://www.yarp.it/yarpmanager.html#module).

###### Documentation generated from xml
Once you have your documentation ready, you can add a simple script in your doxygen folder, e.g. [doc-compile.sh for Linux](https://github.com/robotology-playground/how-to-document-modules/blob/gh-pages/doxygen/doc-compile.sh) or [doc-compile.ps1 for Windows](https://github.com/robotology-playground/how-to-document-modules/blob/gh-pages/doxygen/doc-compile.ps1).
In both the scripts, the first part allows the automatically generation of documentation from the xml file. This documentation will be put inside the **generated-from-xml** folder and this is the reason why we need to include **generated-from-xml** in the generate.txt.

Thus, you can compile or update your documentation just typing:

- for Linux users:
```
cd doxygen/
sh doc-compile.sh
```
- for Windows users:

  ```
  1. Launch PowerShell
  2. Navigate to the directory where the script lives:
  3. Execute the scritp: .\my_script.ps1

  or

  you can run the PowerShell script from cmd.exe like this:
  powershell -noexit "& ""C:\my_path\my_script.ps1""" (enter)

  ```

### Why did we include also the idl_dox folder? (TEMPORARY WORKAROUND)
This folder is necessary only if you have implemented the thrift services. In this case, the file **modulename_IDL.h** will be generated automatically inside the **build** folder. Ignoring the build folder in github repo is a good practice. Then, How can we track the **modulename_IDL.h** file if it is in the build folder?
Currently, the solution is roughly provided by a simple `paste and copy` step of that file inside the idl_dox folder. This is just a **temporary workwaround** and a better solution will be provided soon!

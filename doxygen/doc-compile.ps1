
$SOURCE="../"
$DESTINATION="generated-from-xml"

if (test-path doc) {
    rm doc -Recurse
}

if (test-path $DESTINATION) {
    rm $DESTINATION -Recurse
}

if (!(test-path $DESTINATION)) {
    mkdir $DESTINATION
}

ls -Path $SOURCE -Filter *.xml -Recurse | `
foreach-object {xsltproc --output "$DESTINATION/$($_.basename).dox" `
$env:YARP_SOURCE_DIR\scripts\yarp-module.xsl $_.fullname}

doxygen ./generate.txt

# we are in Resources/Deployment/macOS/ -> change directory to project root
cd ../../../

# set convenience variables
AppBundlePath=Builds/MacOSX/build/Release/AES70_OCP1_StringGenerator.app
CreateDmgPath=submodules/create-dmg/create-dmg
IconSetSourcePng=AES70_OCP1_StringGenerator.png
LicensePath=LICENSE
VolIconPath=Resources/Images/Iconset.icns
DmgTargetPath=AES70_OCP1_StringGenerator.dmg
DmgContentsCollectionPath=ContentsPath

# create the icns from existing png
cd Resources/Images
chmod +x makeIconset.sh
./makeIconset.sh "$IconSetSourcePng"
cd ../../

# collect dmg contents
test -d "$DmgContentsCollectionPath" && rm -r "$DmgContentsCollectionPath"
mkdir "$DmgContentsCollectionPath"
cp -r "$AppBundlePath" "$DmgContentsCollectionPath"
ln -s /Applications "$DmgContentsCollectionPath"/Applications

# create project disk image
test -f "$DmgTargetPath" && rm "$DmgTargetPath"
"$CreateDmgPath" --eula "$LicensePath" --window-size 565 245 --volicon "$VolIconPath" "$DmgTargetPath" "$DmgContentsCollectionPath"

# cleanup
test -d "$DmgContentsCollectionPath" && rm -r "$DmgContentsCollectionPath"
test -f "$VolIconPath" && rm "$VolIconPath"
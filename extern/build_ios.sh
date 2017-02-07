


BUILD_DIR="./lib/ios/"
SDK_VERSION=7.1


cd ./freetype/
xcodebuild clean 	-configuration Release  ONLY_ACTIVE_ARCH=NO  BUILD_DIR="${BUILD_DIR}"  -sdk iphonesimulator${SDK_VERSION}
xcodebuild -alltargets -configuration Release  ONLY_ACTIVE_ARCH=NO  BUILD_DIR="${BUILD_DIR}" -sdk iphonesimulator${SDK_VERSION} 
xcodebuild clean 	-configuration Release  ONLY_ACTIVE_ARCH=NO  BUILD_DIR="${BUILD_DIR}"  -sdk iphoneos${SDK_VERSION}
xcodebuild -alltargets -configuration Release  ONLY_ACTIVE_ARCH=NO  BUILD_DIR="${BUILD_DIR}"  -sdk iphoneos${SDK_VERSION}
xcodebuild clean 	-configuration Debug 	 ONLY_ACTIVE_ARCH=NO  BUILD_DIR="${BUILD_DIR}"  -sdk iphonesimulator${SDK_VERSION}
xcodebuild -alltargets -configuration Debug 	 ONLY_ACTIVE_ARCH=NO  BUILD_DIR="${BUILD_DIR}"  -sdk iphonesimulator${SDK_VERSION}
xcodebuild clean 	-configuration Debug 	 ONLY_ACTIVE_ARCH=NO  BUILD_DIR="${BUILD_DIR}"  -sdk iphoneos${SDK_VERSION}
xcodebuild -alltargets -configuration Debug 	 ONLY_ACTIVE_ARCH=NO  BUILD_DIR="${BUILD_DIR}"  -sdk iphoneos${SDK_VERSION}
cd ../



cd ./freeimage/
xcodebuild clean 	-configuration Release  ONLY_ACTIVE_ARCH=NO  BUILD_DIR="${BUILD_DIR}"  -sdk iphonesimulator${SDK_VERSION}
xcodebuild -alltargets -configuration Release  ONLY_ACTIVE_ARCH=NO  BUILD_DIR="${BUILD_DIR}" -sdk iphonesimulator${SDK_VERSION}
xcodebuild clean 	-configuration Release  ONLY_ACTIVE_ARCH=NO  BUILD_DIR="${BUILD_DIR}"  -sdk iphoneos${SDK_VERSION}
xcodebuild -alltargets -configuration Release  ONLY_ACTIVE_ARCH=NO  BUILD_DIR="${BUILD_DIR}"  -sdk iphoneos${SDK_VERSION}
xcodebuild clean 	-configuration Debug 	 ONLY_ACTIVE_ARCH=NO  BUILD_DIR="${BUILD_DIR}"  -sdk iphonesimulator${SDK_VERSION}
xcodebuild -alltargets -configuration Debug 	 ONLY_ACTIVE_ARCH=NO  BUILD_DIR="${BUILD_DIR}"  -sdk iphonesimulator${SDK_VERSION}
xcodebuild clean 	-configuration Debug 	 ONLY_ACTIVE_ARCH=NO  BUILD_DIR="${BUILD_DIR}"  -sdk iphoneos${SDK_VERSION}
xcodebuild -alltargets -configuration Debug 	 ONLY_ACTIVE_ARCH=NO  BUILD_DIR="${BUILD_DIR}"  -sdk iphoneos${SDK_VERSION}
cd ../


echo "**IOS COPY UI SUCCEEDED! **"




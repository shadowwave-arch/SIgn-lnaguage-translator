# 🛠️ COMPLETE SETUP GUIDE
## Sign Language Translator — Windows Installation

> **You're a beginner? Perfect. Follow every step exactly.**
> Don't skip any step. If something fails, check the Troubleshooting section at the bottom.

---

## 📋 WHAT YOU NEED TO INSTALL

| Tool | Purpose | Download Link |
|------|---------|---------------|
| Visual Studio 2022 (Community) | C++ Compiler + Build Tools | https://visualstudio.microsoft.com/downloads/ |
| CMake 3.25+ | Build system generator | https://cmake.org/download/ |
| Git | Version control | https://git-scm.com/download/win |
| OpenCV 4.8+ | Computer vision library | https://opencv.org/releases/ |
| MediaPipe C++ | Hand detection library | See Step 4 |
| VS Code | Code editor | https://code.visualstudio.com/ |
| vcpkg | C++ package manager | https://github.com/microsoft/vcpkg |

**Estimated setup time: 2–4 hours** (most of it is downloading)

---

## 🔢 STEP 1: Install Visual Studio 2022

Visual Studio provides the **MSVC compiler** (the thing that turns your C++ code into an .exe).

1. Download **Visual Studio 2022 Community** (it's free)
2. Run the installer
3. In the **Workloads** screen, check:
   - ✅ **Desktop development with C++**
   - ✅ **C++ CMake tools for Windows** (inside the right panel)
4. Click Install (this downloads ~7GB)
5. After install, open **"x64 Native Tools Command Prompt for VS 2022"**
   - Search for it in Start Menu
   - This is the terminal you'll use for building!

**Why MSVC?** It's the standard Windows compiler and works best with OpenCV/MediaPipe on Windows.

---

## 🔢 STEP 2: Install CMake

1. Download the Windows installer from https://cmake.org/download/
   - Get the `.msi` file (e.g., `cmake-3.28.0-windows-x86_64.msi`)
2. Run installer → check **"Add CMake to the system PATH for all users"**
3. Verify: open Command Prompt and type:
   ```
   cmake --version
   ```
   Should print: `cmake version 3.28.x`

---

## 🔢 STEP 3: Install OpenCV

OpenCV is the image processing library we use for webcam access, drawing, and the SVM classifier.

### Method A: Pre-built installer (easiest)
1. Go to https://opencv.org/releases/
2. Download `opencv-4.8.0-windows.exe` (or latest 4.x version)
3. Run it — it will extract to `C:\opencv\`
4. Add to PATH:
   - Press `Win + X` → System → Advanced System Settings → Environment Variables
   - Under **System Variables**, find `Path` → Edit → New
   - Add: `C:\opencv\build\x64\vc16\bin`
   - Click OK everywhere
5. Verify: in Command Prompt:
   ```
   dir C:\opencv\build\include\opencv2
   ```
   Should list OpenCV headers.

### Method B: Build from source (if Method A doesn't work)
```bash
git clone https://github.com/opencv/opencv.git
cd opencv
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A x64 \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_LIST=core,imgproc,highgui,videoio,ml \
  -DCMAKE_INSTALL_PREFIX=C:/opencv
cmake --build . --config Release --target INSTALL
```

---

## 🔢 STEP 4: Install MediaPipe C++

MediaPipe is the most complex dependency. Follow this carefully.

### Understanding MediaPipe build options:

**OPTION A: Use Bazel (Google's official method)**
```bash
# Install Bazel first: https://bazel.build/install/windows
# Then:
git clone https://github.com/google/mediapipe.git
cd mediapipe
bazel build -c opt //mediapipe/graphs/hand_tracking:hand_tracking_desktop_live
```

**OPTION B: Use the pre-built MediaPipe C++ SDK (recommended for beginners)**

1. Download mediapipe-cpp from: https://github.com/google-ai-edge/mediapipe
   Or check: https://github.com/PINTO0309/mediapipe_prebuilt (pre-built binaries)

2. Extract to `C:\mediapipe\`

3. Expected structure:
   ```
   C:\mediapipe\
   ├── mediapipe\          (header files)
   │   ├── framework\
   │   │   └── calculator_framework.h
   │   ├── formats\
   │   └── graphs\
   └── build\
       └── Release\
           └── mediapipe_framework.lib
   ```

**OPTION C: Use vcpkg (easiest if it works)**
```bash
# Install vcpkg first (see Step 5)
vcpkg install mediapipe:x64-windows
```

### Download the required model files:
MediaPipe needs pre-trained neural network model files to detect hands.

```bash
# Create the models/mediapipe directory
mkdir C:\mediapipe\models

# Download hand detection model (palm detection)
curl -L "https://storage.googleapis.com/mediapipe-models/hand_detector/palm_detection_full/float16/latest/palm_detection_full.tflite" \
     -o "C:\mediapipe\models\palm_detection_full.tflite"

# Download hand landmark model
curl -L "https://storage.googleapis.com/mediapipe-models/hand_landmarker/hand_landmarker/float16/latest/hand_landmarker.task" \
     -o "C:\mediapipe\models\hand_landmarker.task"
```

---

## 🔢 STEP 5: Install vcpkg (C++ Package Manager)

vcpkg makes it easy to install Abseil (absl) and Protobuf which MediaPipe needs.

```bash
# Open x64 Native Tools Command Prompt (from Visual Studio)
cd C:\
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat

# Install required packages
.\vcpkg install abseil:x64-windows
.\vcpkg install protobuf:x64-windows

# Integrate with CMake (important!)
.\vcpkg integrate install
```

This last command will print something like:
```
CMake projects should use: -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
```
**Save this path! You'll need it in the CMake configure step.**

---

## 🔢 STEP 6: Install VS Code and Extensions

1. Download from https://code.visualstudio.com/
2. Install these extensions (Ctrl+Shift+X to open Extensions):
   - **C/C++** by Microsoft (ms-vscode.cpptools)
   - **CMake Tools** by Microsoft (ms-vscode.cmake-tools)
   - **C/C++ Extension Pack** (ms-vscode.cpptools-extension-pack)
   - **GitLens** (optional but useful)

---

## 🔢 STEP 7: Clone/Open the Project

```bash
# Open x64 Native Tools Command Prompt
cd C:\Users\YourName\Projects

# If you got this as a zip, just extract it.
# If you're cloning from git:
git clone https://github.com/yourusername/sign-language-translator.git
cd sign-language-translator
```

Open VS Code in the project folder:
```bash
code .
```

---

## 🔢 STEP 8: Configure Build

1. In VS Code, press **Ctrl+Shift+P** → type "CMake: Configure"
2. Select **Visual Studio 2022 Release - amd64**
3. If it asks for the toolchain file, enter:
   ```
   C:/vcpkg/scripts/buildsystems/vcpkg.cmake
   ```

**OR** configure manually from the terminal:
```bash
# From your project root
mkdir build
cd build

cmake .. ^
  -G "Visual Studio 17 2022" -A x64 ^
  -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake ^
  -DOpenCV_DIR=C:/opencv/build ^
  -DMEDIAPIPE_ROOT=C:/mediapipe ^
  -DMEDIAPIPE_BUILD=C:/mediapipe/build/Release ^
  -DCMAKE_BUILD_TYPE=Release
```
(`^` is the Windows line continuation character)

---

## 🔢 STEP 9: Build the Project

```bash
# From the build directory
cmake --build . --config Release --parallel

# OR from VS Code:
# Press Ctrl+Shift+B (runs the default build task)
```

If successful, you'll see:
```
[100%] Linking CXX executable Release\SignLanguageTranslator.exe
Build succeeded.
```

---

## 🔢 STEP 10: Run the Application

```bash
# From the PROJECT ROOT (not build/)
# This is important! The app looks for data/ and models/ here.
.\build\Release\SignLanguageTranslator.exe
```

You should see a webcam window open with the hand tracking overlay.

---

## 📊 STEP 11: Collect Training Data

1. Run the app
2. Make the **letter A** sign (make a fist with thumb on the side)
3. Press **C** to enter collection mode
4. Press **SPACE** 50-100 times while holding the gesture steadily
5. Press **A** (keyboard) to switch to label 'A' if it isn't already
6. Press **B** on keyboard to switch to label B
7. Make the **letter B** sign, press SPACE 50-100 times
8. Repeat for all letters you want to recognize
9. Press **C** again to stop collecting

**Tips for good training data:**
- Vary the distance from camera (close, medium, far)
- Vary the angle slightly (not robot-perfect, real variations)
- Vary lighting (if possible, collect in different lighting)
- Collect at LEAST 50 samples per letter (100-200 is better)

---

## 🧠 STEP 12: Train the SVM

After collecting data, press **T** in the running application.

OR run from terminal (if you add a train-only mode):
```bash
.\build\Release\SignLanguageTranslator.exe --train-only
```

Training takes 30-120 seconds. When done, you'll see:
```
[SVMTrainer] Training complete!
[SVMTrainer] Optimal C: 8.0
[SVMTrainer] Optimal Gamma: 0.125
[SVMTrainer] Training accuracy: 96.5%
[SVMTrainer] Model saved to: models/svm_model.xml
```

---

## 🎯 STEP 13: Predict Signs

After training, the model loads automatically on next launch.
Show your hand signs to the camera and see predictions!

---

## 🔧 TROUBLESHOOTING COMMON ERRORS

### Error: "OpenCV not found"
```
Solution: Set OpenCV_DIR explicitly in CMake:
cmake .. -DOpenCV_DIR="C:/opencv/build"
```

### Error: "Cannot open webcam"
```
Solutions:
1. Check Device Manager → Cameras (is it listed?)
2. Try changing WEBCAM_ID from 0 to 1 in main.cpp
3. Allow camera access in Windows Privacy Settings
4. Update camera drivers
```

### Error: "Unresolved external symbol" during linking
```
Solutions:
1. Make sure you're using x64 build (not x86)
2. Check that OpenCV_DIR points to the build folder, not the install folder
3. Ensure MSVC version matches the OpenCV prebuilt (vc15 vs vc16)
```

### Error: "DLL not found" when running
```
Solutions:
1. Copy all OpenCV DLLs from C:\opencv\build\x64\vc16\bin\ 
   to your build\Release\ directory
2. OR add C:\opencv\build\x64\vc16\bin to your PATH
```

### Error: MediaPipe graph initialization failed
```
Solutions:
1. Check that model .tflite files are in the correct location
2. MediaPipe needs specific model files — download them (see Step 4)
3. Try running from the project root directory, not build/
```

### Poor prediction accuracy
```
Solutions:
1. Collect more training data (200+ samples per letter)
2. Ensure consistent lighting
3. Retrain after collecting more data
4. Check that your hand is well within the frame
```

---

## 📁 EXPECTED FOLDER STRUCTURE AFTER SETUP

```
sign_language_translator/
├── .vscode/
│   ├── settings.json
│   ├── launch.json
│   └── tasks.json
├── assets/
├── build/              ← Created by CMake
│   ├── Debug/
│   └── Release/
│       └── SignLanguageTranslator.exe  ← Your compiled app!
├── data/
│   ├── samples_A.csv   ← Created when you collect letter A
│   ├── samples_B.csv
│   └── ... etc
├── include/
│   ├── hand_detector.hpp
│   ├── feature_extractor.hpp
│   ├── predictor.hpp
│   ├── svm_trainer.hpp
│   └── utils.hpp
├── models/
│   └── svm_model.xml   ← Created after training
├── src/
│   ├── main.cpp
│   ├── hand_detector.cpp
│   ├── feature_extractor.cpp
│   ├── svm_trainer.cpp
│   ├── predictor.cpp
│   └── utils.cpp
├── CMakeLists.txt
├── SETUP_GUIDE.md
└── README.md
```

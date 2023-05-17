## Showcase

![](screenshots/demo_chat.gif "The chat mode.")
> Chat with a cute and live ChatGPT!

![](screenshots/demo_holo1.gif "The holo mode.")
> This is the fullscreen holo mode. It is used to project the hologram using a pyramid!

![](screenshots/demo_holo2.gif "The holo mode.")
> This is the actual hologram displayed using the pyramid! The expression is changing.

## Table of Content<a name="table-of-content"></a>

+ [TO-DO](#to-do)
+ [Introduction](#introduction)
	+ [Prerequisites](#prerequisites)
+ [Establishing the Environment](#establishing-the-environment)
+ [Build and Deploy](#build-and-deploy)
	+ [Windows](#windows)
	+ [Linux](#linux)
+ [Usage](#usage)
	+ [UI](#ui)
	+ [Settings](#settings)
	+ [Hologram](#hologram)
+ [Live2D Model](#live2d-model)
+ [Pipeline](#pipeline)
+ [End Note](#end-note)

## TO-DO<a name="to-do"></a>
[Return to Table of Content](#table-of-content)

- [ ] Configure `.pro` file for compilation in Linux.
- [ ] Add more models.


## Introduction<a name="introduction"></a>
[Return to Table of Content](#table-of-content)


After months' thinking, I come up with a practical and controllable idea for the proposal!

+ Develop a ChatGPT-based Live2D chatbot with hologram display.
	+ The development shall base on Qt6/C++, which is totally new to me.
	+ The Live2D chatbot shall either dock on the screen as a standard Live2D widget, or display in the holographic mode on a hologram pyramid.
	+ The Live2D chatbot shall support voice and keyboard interaction, with natural expressions and even body motions.
	+ The whole development process shall be well-archived and reproducible.

The main idea goes like this. A cute [Live2D](https://www.live2d.com/en/) character (I shall call it Live2D model, or model for convenience) will stay on the bottom right of the user's screen. The model is nothing more than a common [Live2D widget](https://github.com/akiroz/Live2D-Widget), except for the fact that it is capable of vocal and bodily communication. And it can even turn into a hologram model once you activate the holo mode! Conceptually speaking it is similar to the [GateBox](https://www.gatebox.ai/), which is a famous virtual intelligent companion with hologram display as well. That said, we are actually DIY an (almost) free and personalized GateBox.

#### Prerequisites<a name="prerequisites"></a>
[Return to Table of Content](#table-of-content)

All the prerequisites are listed below.

First things first, a [PC](https://en.wikipedia.org/wiki/Personal_computer) is required. The development environment can be [Windows 10/11](https://www.microsoft.com/software-download/windows11) + [MSVC compiler](https://visualstudio.microsoft.com/vs/older-downloads/#visual-studio-2019-and-other-products) or [Ubuntu20/22](https://ubuntu.com/download/desktop) + [GCC compiler](https://linuxize.com/post/how-to-install-gcc-on-ubuntu-20-04/), with the [Qt Creator](https://github.com/qt-creator/qt-creator) as the [IDE](https://en.wikipedia.org/wiki/Integrated_development_environment). Also, a mirror APP is needed so that you can mirror your PC screen to a mobile device for hologram display (using a pyramid) through Wifi. A simple google (maybe new Bing nowadays) search can find you plenty of such apps.

Second, an [OpenAI API key](https://platform.openai.com/docs/introduction) for chatGPT calls and a [Microsoft Azure API key](https://learn.microsoft.com/en-us/azure/search/search-security-api-keys?tabs=portal-use%2Cportal-find%2Cportal-query) for speech-to-text and text-to-speech services are required. When you speak to the Live2D character, your words will be conversed to texts using Azure's speech-to-text API and sent to chatGPT. The latter's response will be sent to Azure's text-to-speech service so that the model can "speak".

Third, a [hologram pyramid](https://www.wikihow.com/Make-a-Holographic-Illusion-Pyramid) and a flat screen are required. The hologram pyramid can be easily found from multiple online shopping platforms. The pyramid's size depends on the screen size. For the latter, a 10'-12' iPad would be the best choice because its size, well-designed appearance, and functionality are just so perfect! Anyway, choose your pyramid based on your screen!

Fourth, a microphone and speaker. 

Some optional requirements include a Raspberry PI or Nvidia Jetson to make things more nerdy and geeky, and your personal chat log with the target user so that a more **you** companion can be created through chatGPT finetuning.

## Establishing the Environment<a name="establishing-the-environment"></a>
[Return to Table of Content](#table-of-content)

We now set up everything before diving into the coding part. First, browse the online shopping platforms and by a hologram pyramid for a 10'-12' screen. You may also [build one manually](https://www.wikihow.com/Make-a-Holographic-Illusion-Pyramid). The pyramid may take one to two weeks to deliver. Also, if you do not have a iPad, then buy or find a LED screen (if you don't mind the annoying wires).

On your PC, there are several things to download/install, they are listed below.
	
+ [Visual Studio and MSVC compiler](https://visualstudio.microsoft.com/vs/older-downloads/#visual-studio-2019-and-other-products) for Windows PC or [GCC compiler](https://linuxize.com/post/how-to-install-gcc-on-ubuntu-20-04/) for Ubuntu PC. When install the Visual Studio, tick "Desktop Development with C++".
+ [Qt Creator](https://github.com/qt-creator/qt-creator), which is an IDE specifically for Qt. It will be used to build the project with the MSVC (Windows) or GCC (Linux) compiler.
+ [Git Bash](https://git-scm.com/download/win) (Windows only), we will use it to run the bat script on Windows. When install, simply click Next, no need to change the default setting except for the installation path. If you are using Linux, install the Git command.
+ [System Environment Variable](https://www.computerhope.com/issues/ch000549.htm), add [OpenAI API key](https://platform.openai.com/docs/introduction) and [Microsoft Azure API key](https://learn.microsoft.com/en-us/azure/search/search-security-api-keys?tabs=portal-use%2Cportal-find%2Cportal-query) as your environment variables. The two keys will be free for one month if you are new user. After which, they will switch to pay-as-you-go. Their charge is trivial, therefore, it does no harm to use them. (During the whole (1.5 month) development of this project, I was only charged for about 3 dollars in total.)
+ [Mirror APP (optional)], which mirror your screen to an iPad. It is required if you don't have a separated screen connected with your PC.

## Build and Deploy<a name="build-and-deploy"></a>
[Return to Table of Content](#table-of-content)

(You don't need this if you just download the released app.) To build the project, there are two major steps. 

#### Windows<a name="windows"></a>
[Return to Table of Content](#table-of-content)

First, locate the path of the `cmake.exe` in your Qt. For me, it is in `E:\Qt\Tools\CMake_64\bin\cmake.exe`. Then, edit `build_component.bat`, adjust Line 4 to

```
set CMAKE=path\to\your\cmake.exe
```

Then, launch `Git Bash`, `cd` to the project root path, run the `build_component.bat` script in the Git Bash by

```
./build_component.bat
```

The batch script will download, build, and install OpenSSL, Curl, and OpenCV, and finally delete all unnecessary resources.


Second, launch your Qt Creator, Open Project --> locate the project folder --> choose the `.pro` file to open. Then click `Run` button on the bottom left. An executable `HoloBot.exe` will be generated (inside something like `build-HoloLive2dChatbot-Desktop_Qt_6_5_0_MSVC2019_64bit-Release`). And we are ready to deploy. 

First, ensure that `VCINSTALLDIR` is set in your [System Environment Variable](https://www.computerhope.com/issues/ch000549.htm). I set it to `D:\VisualStudio2019\VC` as I installed the VS to disk D. Then, locate the path of the `windeployqt.exe` in your Qt. For me, it is in `E:\Qt\6.5.0\msvc2019_64\bin\windeployqt.exe`. Make sure its version (6.5.0) is the same as your working Qt version. There could be multiple `windeployqt.exe` existed in your Qt. For me, I have another one of version `6.4.1` in `E:\Qt\Tools\QtDesignStudio\qt6_design_studio_reduced_version\bin` which is not compatible with my Qt 6.5.0. 

Second, copy `HoloBot.exe` to an empty folder, lets call it `\HoloBot\HoloBot.exe`, in that folder, launch Git Bash, copy all the dependencies to this folder by

```
E:\Qt\6.5.0\msvc2019_64\bin\windeployqt.exe . --compiler-runtime
``` 

Make sure there is no error returned. The command will generate the following dependencies.

![](screenshots/deploy.png "The generated dependencies.")
> All except `HoloBot.exe` are automatically generated. Make sure `vc_redist_x64.exe` is there!




Finally, manually copy the follows to `\HoloBot\`.

+ `libcurl.dll`
+ `Microsoft.CognitiveServices.Speech.core.dll`
+ `Microsoft.CognitiveServices.Speech.extension.audio.sys.dll`
+ `Microsoft.CognitiveServices.Speech.extension.codec.dll`
+ `opencv_core470.dll`, `opencv_highgui470.dll`, `opencv_imgcodecs470.dll`, `opencv_imgproc470.dll`
+ `Resources` folder (don't forget this!)


Now `\HoloBot` is a standalone package for the app!!


#### Linux<a name="linux"></a>
[Return to Table of Content](#table-of-content)

To do...


## Usage<a name="usage"></a>
[Return to Table of Content](#table-of-content)

#### UI<a name="ui"></a>
[Return to Table of Content](#table-of-content)

The UI of the chatbot is shown below.

![](screenshots/UI.png "The UI of the bot")
> The UI of the chatbot. The toolbar appears only if the mouse is hover on the model.

The toolbar, from left to right, contains the button for the following functions.

+ Close, cancellation of the program.
+ Reset, clear the chat history. The chat history is necessary for ChatGPT to track the context of the conversation. Reset it if you want to restart the conversation.
+ Widget mode, the normal Live2D widget display with the model being docked at the bottom right of the screen.
+ Chat mode, the keyboard-based conversation with ChatGPT. Sharing the chat history with the Talk mode.
+ Holo mode, the duplicated frame (by rotating it for 0, 90, 180, and 270 degree) for hologram pyramid display.
+ Talk mode, the vocal conversation with ChatGPT. Click it and speak to ChatGPT, click again to stop the conversation. The Azure speech-to-text and text-to-speech service are used. Sharing the chat history with the Chat mode.
+ Settings, the dialog to choose different Live2D model, the prompts to instruct chatGPT, and the language for Azure service. Currently only 2 Live2D models (Haru and Wanko) are added. Instructions to add more models are details later.
+ Help, the message box instructing the key bindings.

#### Settings<a name="settings"></a>
[Return to Table of Content](#table-of-content)

There are two steps to set the app.

+ First, edit the `languages.ini` file.
+ Second, interact with the `Settings` on the app toolbar.

The `languages.ini` defines the language and voice for the Azure text-to-speech API. It looks like this.

```
[1]
voice=en-US-JennyNeural

[2]
voice=zh-CN-XiaoyiNeural
```

You may

1. add more language, 
2. switch the order of them (put your desired under \[1\] for convenience, but make sure you follow the format and assign one language for one group), and 
3. choose a different voice. 
	+ To choose a different voice, visit the [voice gallery](https://speech.microsoft.com/portal/193919d8d6294e799745bbe666cdea3c/voicegallery) from Azure website. Once you chose a voice, you should see the corresponding example code, from which you can find the voice name, for example, `en-US-GuyNeural`, then add it to `languages.ini`. 

The `Settings` tool looks like this.

![](screenshots/Settings.png "The Settings dialog of the bot.")
> The Setting dialog of the chatbot. From which you can choose the Live2D model, the prompt txt file, and languages. The languages are retrieved from `languages.ini`.

The prompt is the key to make ChatGPT act as your needs. In the default prompt, I asked ChatGPT  to act as Einstein, reply in JSON format, and speak in Chinese. You may also noticed how I instruct ChatGPT to format the JSON. The `Expression` and `Motion` are retrieved from Live2D model json. **Except for the two lines regarding `Expression` and `Motion` instruction, you can alter it as you wish.** After which a new prompt will be saved and available to select via the Prompt combobox. Make sure to change the language in the last line to be consistent with the Language combobox. (For example, change it to `In Content, you must speak in English.` should you chose `en-US-JennyNeutral`.)


#### Hologram<a name="hologram"></a>
[Return to Table of Content](#table-of-content)

Suppose you have the hologram pyramid, there are several ways to display it depending on your case.

1. You have a PC and an iPad: install a mirror app in your PC, and then mirror your screen to an iPad. (There are numerous mirror apps available online, such as this [awesome one](https://github.com/H-M-H/Weylus). See [this issue](https://github.com/H-M-H/Weylus/issues/140) if you use Win11.) The downside is that iPad supports 4:3 ratio whereas normal monitor is usually in 16:9. Therefore we have to adjust the PC resolution to 4:3 to mirror it properly.
2. You have a PC and a flat LED screen: simply connect the screen to your PC. The good thing is that the LED screen can mostly support 16:9 ratio.
3. You have a Raspberry PI or Nvidia Jetson, on which placed embedded a small screen: this makes the who system portable.

I am currently using Option 1. No matter which one you choose, set the app as mentioned above, enable the talk mode, then the Holo mode, place the screen on the pyramid (or place the pyramid on your screen, should you use a heavy screen), and enjoy!

(If you mirror the Live2D model via Safari on iPad, make sure to [make it shown full screen](https://www.ispringsolutions.com/blog/how-to-make-a-website-full-screen-on-the-ipad) manually.)


## Live2D Model<a name="live2d-model"></a>
[Return to Table of Content](#table-of-content)

Before import a new model, you need to do the follows.

+ Download a model from the [official collection](https://www.live2d.com/en/download/sample-data/) or any 3rd party sources. Make sure there is a `xxx.model3.json` in it, not a `xxx.model.json`. The latter only supports  Live2D SDK version <3.0 (I guess), whereas the app is based on the newest (4.2.4) Live2D SDK.
+ Edit `xxx.model3.json` for your model. The goal is to name each expressions and motions meaningfully so that later ChatGPT can react based on them. In some models, the expressions or motions may have meaningless name such as `F01`, `F02`, etc. Change it to meaningful name such as `Happy`, `Sad`. 
	+ How to determine the correct name? Go to an Live2D online viewer like [this one](https://guansss.github.io/live2d-viewer-web/), drag your model folder to it and play each expression and/or motion. By which you should be able to eyeball and name them. Make them distinguishable to each other. For example, for expressions, there are several [basic emotions](https://en.wikipedia.org/wiki/Emotion) to name, including `Happy`, `Sad`, `Disgusting`, `Fear`, `Angry`, `Neutral`, `Surprised`, and `Contempt`, and for motions, consider `Smile`, `Cry`, `Laugh`, `Doubt`, `Jump`, etc... Restructure the json if necessary. See the existing model json files (such as `Resources/Wanko_touch/Wanko_touch.model3.json` and `Resources/Haru/Haru.model3.json`) for reference.
+ If the model's predefined expressions and motions do not satisfy you, you can use [Live2D Editor](https://www.live2d.com/en/) to create more. I am very new to this but there are plenty of tutorials online. Alternatively, you may pay someone to create a custom model for you.

To import more models, simply copy/paste the model folder under `Resources` folder. The model structure looks like this.

![](screenshots/model_structure.png "The structure of a model.")
> Usually, a model folder contains the expressions, textures, motions, sounds, and some other files. Some may have only one of expressions or motions. Make sure your model folder is consistent with the model json file. Here in this example, the model folder `Haru` and the model json file `Haru.model3.json` have the a `Haru` in them. Inconsistency of this would crash the app.

Restart the app to load the imported model.

## Pipeline<a name="pipeline"></a>
[Return to Table of Content](#table-of-content)

![](screenshots/pipeline.jpg "The pipeline of the app.")
> The input, should it be voice, will be converted to text by Azure. It is then combined to form the "messages" for OpenAI API format. The output from ChatGPT is in JSON format, containing three keys: "Expressions", "Motions", and "Content". The "Expressions" and "Motions" are used to animate the Live2D model, and the "Content" will be voiced by Azure and also stitched to the "messages". Loop until the user terminates the talk session. The chat history is temporarily saved and will be read if user restart the chat or talk session, unless the user resets the history or closes the app.


## End Note<a name="end-note"></a>
[Return to Table of Content](#table-of-content)

Thanks [duanxianpi](https://github.com/duanxianpi/QtLive2d) for creating the QtLive2D Widget and his timely support. Thanks [ChatGPT](https://chat.openai.com/) for the super helpful guidance on each technical problems. Frankly speaking I know nothing about C++ and Qt. It is ChatGPT who helped me realize this app. (No, I write this ReadMe by myself :)

Please excuse any inappropriate C++ coding and English writing from my inexperience.











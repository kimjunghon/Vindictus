// 명령어			옵션			원본 파일이 있는 위치			사본 파일을 저장할 위치

xcopy			/y/s		.\Engine\Public\*.*					.\EngineSDK\Inc\

xcopy			/y			.\Engine\Bin\Debug\Engine.dll		.\Client\Bin\Debug\
xcopy			/y			.\Engine\Bin\Debug\Engine.dll		.\Edit\Bin\Debug\
xcopy			/y			.\Engine\Bin\Debug\Engine.lib		.\EngineSDK\Lib\Debug\

xcopy			/y			.\Engine\Bin\Release\Engine.dll		.\Client\Bin\Release\
xcopy			/y			.\Engine\Bin\Release\Engine.dll		.\Edit\Bin\Release\
xcopy			/y			.\Engine\Bin\Release\Engine.lib		.\EngineSDK\Lib\Release\

xcopy			/y			.\Engine\Bin\Shaderfiles\*.*		.\Client\Bin\ShaderFiles\
xcopy			/y			.\Engine\Bin\Shaderfiles\*.*		.\Edit\Bin\ShaderFiles\
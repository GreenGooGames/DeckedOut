@echo off
setlocal enabledelayedexpansion
REM /////////////////////////////////////////
REM Parameter setup
REM /////////////////////////////////////////

REM Fill in these variables to fit your project.
set PROJECT_DIRECTORY=E:\Unreal Projects\DeckedOut
set ENGINE_DIRECTORY=C:\Program Files\Epic Games\UE_5.4
set TARGET=Tartarus

REM Fill in relative to the PROJECT_DIRECTORY.
set UPROJECT_PATH=DeckedOut\Tartarus.uproject
set ARCHIVE_DIRECTORY=!PROJECT_DIRECTORY!\DeckedOut\Packages

REM Fill in relative to the ENGINE_DIRECTORY.
set UAT_PATH=Engine\Build\BatchFiles\RunUAT.bat

REM Setup values to be used later on, DO NOT MODIFY.
set DRIVE_LETTER=%ENGINE_DIRECTORY:~0,2%

REM Default parameter group definitions, DO NOT MODIFY.
set BASE_PARAMS=-noP4 -unattended -utf8output -Messaging -nocodesign -unversionedcookedcontent
set BUILD_PARAMS=-compile -build -cook -package -stage -archive
set SPEED_PARAMS=-FastCook -iterate
set SIZE_PARAMS=-compressed "-AdditionalCookerOptions=\"-nodev\""

REM Data validation inputs, DO NOT MODIFY.
set VALID_PRESETS=w64bug w64buggame w64dev w64devpak w64ship w64shippak LnxDevServer configure
set VALID_PLATFORMS=win64 ps4 ps5 xboxone xsx Linux
set VALID_CONFIGS=Debug DebugGame Development Test Shipping Server
set VALID_YESNO=y n

REM Leave these params empty, will be filled later depending on the configuration, DO NOT MODIFY.
set PLATFORM=
set CONFIG=
set CLIENT_PARAMS=
set SERVER_PARAMS=
set PAK_PARAMS=
set CLEAN_PARAMS=
set ARCHIVE_PARAMS=
set BUILDCONFIG_PARAMS=

REM Setup params for presets, DO NOT MODIFY.
REM Preset_Name=[Platform] [Config] [Clean] [Pak] [Server]
set PRESET_w64bug=win64 debug n n n
set PRESET_w64buggame=win64 debuggame n n n
set PRESET_w64dev=win64 development n n n
set PRESET_w64devpak=win64 development n y n
set PRESET_w64ship=win64 shipping n n n
set PRESET_w64shippak=win64 shipping n y n
set PRESET_LnxDevServer=linux development n n y

REM /////////////////////////////////////////
REM Main body
REM /////////////////////////////////////////
:Main
    :: Validate input from the user.
    call :PresetInput
    
    if !PRESET_INPUT!==configure (
        call :CleanProjectInput
        call :PlatformInput
        call :ConfigurationInput
        call :PakInput
	call :ServerInput
    )
    
    :: Setup build variables based on the input.
    call :GatherBuildParams
    call :SetupBuildParams !BUILDCONFIG_PARAMS!
    call :SetupArchiveParams
    
    :: Execute the package command.
    call :PackageCommand

    :: End the program.
    goto EndOfProgram

REM Finish the program, make sure these are the last lines in the script.
:EndOfProgram
    cmd /k @pause


REM /////////////////////////////////////////
REM Helpers
REM /////////////////////////////////////////

REM Compares the first parameter to all subsequent parameters and sets the errorlevel based on wether a match is found.
:ValidateInput
    :: Set the first parameter as the input.
    set "Input=%1"
    call :ToLower Input

    :: Shift to the next parameter.
    shift
    
    :ValidateInputLoop
        set "Compare=%1"

        if "%Compare%"=="" (
            :: If the parameter is empty, then we reached the end and the input wasn't part of any option return fail.
            echo "The input is not recognized!"
            exit /b 1
        )
        
        call :ToLower Compare
        if "%Compare%"=="%Input%" (
            :: if the parameter equals a parameter, then exit with success.
            exit /b 0
        )

        :: Shift to the next parameter
        shift
        goto ValidateInputLoop

    exit /b 0

:ToLower
    for %%i IN ("A=a" "B=b" "C=c" "D=d" "E=e" "F=f" "G=g" "H=h" "I=i" "J=j" "K=k" "L=l" "M=m" "N=n" "O=o" "P=p" "Q=q" "R=r" "S=s" "T=t" "U=u" "V=v" "W=w" "X=x" "Y=y" "Z=z") DO CALL SET "%1=%%%1:%%~i%%"
    exit /b 0


REM /////////////////////////////////////////
REM User input
REM /////////////////////////////////////////

REM Allow the user to select a platform and store it.
:PresetInput
    echo -------------------------------------------------------------
    echo Do you want to run a preset or configure?
    echo -------------------------------------------------------------

    set /p PRESET_INPUT=Valid presets are: !VALID_PRESETS!: 
    call :ValidateInput !PRESET_INPUT! !VALID_PRESETS!
    
    if errorlevel 1 (
        call :PresetInput
    )

    exit /b 0

REM Allow the user to select a platform and store it.
:PlatformInput
    echo -------------------------------------------------------------
    echo For which platform do you want to package?
    echo -------------------------------------------------------------
    
    set /p PLATFORM_INPUT=Valid platforms are: !VALID_PLATFORMS!: 
    call :ValidateInput !PLATFORM_INPUT! !VALID_PLATFORMS!
    
    if errorlevel 1 (
        call :PlatformInput
    )

    exit /b 0

REM Allow the user to select a configuration and store it.
:ConfigurationInput
    echo -------------------------------------------------------------
    echo For which configuration do you want to package?
    echo -------------------------------------------------------------

    set /p CONFIG_INPUT=Valid presets are: !VALID_CONFIGS!: 
    call :ValidateInput !CONFIG_INPUT! !VALID_CONFIGS!

    if errorlevel 1 (
        call :ConfigurationInput
    )

    exit /b 0

REM Allow the user to pick if intermediate should be deleted before build.
:CleanProjectInput
    echo -------------------------------------------------------------
    echo Do you want to a clean build?
    echo -------------------------------------------------------------

    set /p CLEAN_INPUT=y (yes) or n (no): 
    call :ValidateInput !CLEAN_INPUT! !VALID_YESNO!
    
    if errorlevel 1 (
        call :CleanProjectInput
    )
    
    exit /b 0

REM Allow the user to pick if they want a .pak file.
:PakInput
    echo -------------------------------------------------------------
    echo Do you want to create a .pak of the build?
    echo -------------------------------------------------------------
    
    set /p PAK_INPUT=y (yes) or n (no): 
    call :ValidateInput !PAK_INPUT! !VALID_YESNO!
    
    if errorlevel 1 (
        call :PakInput
    )

    exit /b 0

REM should we build a server?
:ServerInput
    echo -------------------------------------------------------------
    echo Do you want to build a server?
    echo -------------------------------------------------------------
    
    set /p SERVER_INPUT=y (yes) or n (no): 
    call :ValidateInput !SERVER_INPUT! !VALID_YESNO!
    
    if errorlevel 1 (
        call :ServerInput
    )

    exit /b 0

REM /////////////////////////////////////////
REM Execution
REM /////////////////////////////////////////

REM Run the UAT tool with the defined parameters
:PackageCommand
    %DRIVE_LETTER%
    cd %ENGINE_DIRECTORY%
    cmd /k !UAT_PATH! BuildCookRun -project="!PROJECT_DIRECTORY!\!UPROJECT_PATH!" !CLIENT_PARAMS! !SERVER_PARAMS! !BASE_PARAMS! !BUILD_PARAMS! !ARCHIVE_PARAMS! !PAK_PARAMS! !SPEED_PARAMS! !SIZE_PARAMS! !CLEAN_PARAMS!
    ::echo !UAT_PATH! BuildCookRun -project="!PROJECT_DIRECTORY!\!UPROJECT_PATH!" !CLIENT_PARAMS! !SERVER_PARAMS! !BASE_PARAMS! !BUILD_PARAMS! !ARCHIVE_PARAMS! !PAK_PARAMS! !SPEED_PARAMS! !SIZE_PARAMS! !CLEAN_PARAMS!
    exit /b 0

REM Gather all build params based on preset or input
:GatherBuildParams
    if %PRESET_INPUT%==configure (
        set BUILDCONFIG_PARAMS=!PLATFORM_INPUT! !CONFIG_INPUT! !CLEAN_INPUT! !PAK_INPUT! !SERVER_INPUT!
        exit /b 0
    ) else (
        set PRESETNAME=PRESET_!PRESET_INPUT!
        call set BUILDCONFIG_PARAMS=%%!PRESETNAME!%%
    )
    
    exit /b 0

REM Sets up all build parameters required.
:SetupBuildParams
    :: Add the platform.
    set PLATFORM=%1

    :: Add the configuration.
    set CONFIG=%2

    :: Add clean intermediate folder.
    if "%3"=="y" (
        set CLEAN_PARAMS=-clean
    )

    :: Add pak assets
    if "%4"=="y" (
        set PAK_PARAMS=-pak
    )

    :: Add server vars
    if "%5"=="y" (
        set SERVER_PARAMS=-server -serverplatform=%PLATFORM% -serverconfig=!CONFIG!
    ) else (
        set CLIENT_PARAMS=-platform=%PLATFORM% -clientconfig=!CONFIG!
    )

    exit /b 0

REM setups the build params based on input.
:SetupArchiveParams
    set ARCHIVE_PAK_EXTENSION=Pak
	
    if "%PAK_PARAMS%"=="" (
        set ARCHIVE_PAK_EXTENSION=NoPak
    )

	set ARCHIVE_PARAMS=-archivedirectory="!ARCHIVE_DIRECTORY!\!CONFIG!\!ARCHIVE_PAK_EXTENSION!"

    exit /b 0

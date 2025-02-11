$ErrorActionPreference = "Stop"
$InformationPreference = "Continue"

<#
    .SYNOPSIS
    Asserts whether an error when an external function has thrown an error via LASTEXITCODE.

    .DESCRIPTION
    This function must be called immediately after the external function call.

    .PARAMETER ThrowError
    Specifies whether to throw an error if an error is detected. If no specified a boolean will be returned instead.

    .INPUTS
    None.

    .OUTPUTS
    None.

    .EXAMPLE
    Import-Module ./modules/Build.psd1
    cmake ..
    Assert-ExternalCommandError -ThrowError -Verbose
#>

function Assert-ExternalCommandError {

    [CmdletBinding()]
    [OutputType([system.boolean])]
    param(
        [Parameter(Position = 0, Mandatory = $false)]
        [switch]
        $ThrowError
    )

    Write-Verbose "##[debug]Running Assert-ExternalCommandError..."
    Write-Verbose "##[debug]Parameters:"
    Write-Verbose "##[debug]    ThrowError: $ThrowError"

    if ($LASTEXITCODE -gt 0) {
        if ($ThrowError) {
            Write-Error "##[error]Please resolve the above errors!"
        }

        else {
            Write-Verbose "##[debug]Returning: true"
            return $true
        }
    }

    elseif (-Not $ThrowError) {
        Write-Verbose "##[debug]Returning: false"
        return $false
    }
}

<#
    .SYNOPSIS
    Builds Lane and Object Detection.

    .DESCRIPTION
    Also builds the OpenCV dependency.

    .PARAMETER Platform
    Specifies the platform being run on. Used to cover the edge case of forcing windows to generate for NMake Makefile instead
    of Visual Studio.

    .PARAMETER BuildType
    Specifies whether to build in Debug or Release.

    .PARAMETER BuildDirectory
    Specifies the directory to build in to relative to the repository root.

    .PARAMETER CleanBuild
    Specifies whether to delete the build folders (including OpenCV build folder) before building.

    .PARAMETER Parallel
    Specifies the option to specify a parallel build level. They map to corresponding options of the native build tool.

    .INPUTS
    None.

    .OUTPUTS
    None.

    .EXAMPLE
    Import-Module ./modules/Build.psd1
    Build-CppCodeUsingCMake -Platform macos-latest -BuildType Release -BuildDirectory build -CleanBuild -Parallel 8 -Verbose
#>

function Build-CppCodeUsingCMake {

    [CmdletBinding()]
    param(
        [Parameter(Position = 0, Mandatory = $true)]
        [ValidateSet("macos-latest", "ubuntu-latest", "windows-latest")]
        [string]
        $Platform,

        [Parameter(Position = 1, Mandatory = $true)]
        [ValidateSet("Debug", "Release")]
        [string]
        $BuildType,

        [Parameter(Position = 2, Mandatory = $true)]
        [string]
        $BuildDirectory,

        [Parameter(Position = 3, Mandatory = $false)]
        [switch]
        $CleanBuild = $false,

        [Parameter(Position = 4, Mandatory = $false)]
        [int]
        $Parallel = 1
    )

    Write-Verbose "##[debug]Running Build-CppCodeUsingCMake..."
    Write-Verbose "##[debug]Parameters:"
    Write-Verbose "##[debug]    Platform: $Platform"
    Write-Verbose "##[debug]    BuildType: $BuildType"
    Write-Verbose "##[debug]    BuildDirectory: $BuildDirectory"
    Write-Verbose "##[debug]    CleanBuild: $CleanBuild"
    Write-Verbose "##[debug]    Parallel: $Parallel"

    if ($CleanBuild) {
        if (Test-Path -LiteralPath ./$BuildDirectory/) {
            Write-Information "##[command]Deleting the './$BuildDirectory/' directory..."
            Remove-Item -LiteralPath "./$BuildDirectory/" -Force -Recurse
        }

        else {
            Write-Information "##[warning]The './$BuildDirectory/' directory does not exist"
        }

        if (Test-Path -LiteralPath ./submodules/opencv/build/) {
            Write-Information "##[command]Deleting the './submodules/opencv/build/' directory..."
            Remove-Item -LiteralPath "./submodules/opencv/build/" -Force -Recurse
        }

        else {
            Write-Information "##[warning]The './submodules/opencv/build/' directory does not exist"
        }

        if (Test-Path -LiteralPath ./submodules/opencv/.cache/) {
            Write-Information "##[command]Deleting the './submodules/opencv/.cache/' directory..."
            Remove-Item -LiteralPath "./submodules/opencv/.cache/" -Force -Recurse
        }

        else {
            Write-Information "##[warning]The './submodules/opencv/.cache/' directory does not exist"
        }
    }

    Write-Information "##[section]Building OpenCV..."

    Write-Information "##[command]Changing directory to OpenCV submodule folder..."
    Set-Location -LiteralPath ./submodules/opencv/

    try {
        Write-Information "##[command]Configuring OpenCV..."
        & cmake -S . -B ./$BuildDirectory -D "CMAKE_BUILD_TYPE=$BuildType" -D "BUILD_opencv_world=ON"
        Assert-ExternalCommandError -ThrowError

        Write-Information "##[command]Building OpenCV..."
        & cmake --build ./$BuildDirectory --config $BuildType --parallel $Parallel
        Assert-ExternalCommandError -ThrowError
    }

    catch {
        throw
    }

    finally {
        Write-Information "##[command]Changing directory to repository root..."
        Set-Location -LiteralPath ../..
    }

    Write-Information "##[section]Building Lane and Object Detection..."

    Write-Information "##[command]Configuring Lane and Object Detection..."

    if ($Platform -eq "windows-latest") {
        cmake -S . -B ./$BuildDirectory -G "NMake Makefiles" -D "CMAKE_BUILD_TYPE=$BuildType"
    }

    else {
        cmake -S . -B ./$BuildDirectory -D "CMAKE_BUILD_TYPE=$BuildType"
    }

    Assert-ExternalCommandError -ThrowError

    Write-Information "##[command]Building Lane and Object Detection..."

    & cmake --build ./$BuildDirectory --config $BuildType --parallel $Parallel
    Assert-ExternalCommandError -ThrowError


    Write-Information "##[section]Lane and Object Detection has been successfully built!"
}

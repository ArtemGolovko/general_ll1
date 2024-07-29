$project_root = Split-Path -Path $MyInvocation.MyCommand.Path
$project_name = "general_ll1"
$project_build = "$project_root/out"
$project_bin = "$project_build/bin"
$project_config = "Debug"

#build step

$action = $args[0]

if ($action -eq "build") {
    cmake -S $project_root -B $project_build 

    Push-Location $project_build

    msbuild "$project_name.sln"

    Pop-Location
}


if ($action -eq "run") {
    $rest_args = @()

    $rest_index = [array]::indexof($args, "/args")
    if ($rest_index -ne -1) {
        $rest_args = $args[($rest_index + 1) .. $args.Length]
    }

    echo ""
    echo "& ""$project_bin/$project_config/$project_name.exe"" $rest_args"

    & "$project_bin/$project_config/$project_name.exe" @rest_args
}

if ($action -eq "test") {
    $rest_args = @()

    $rest_index = [array]::indexof($args, "/args")
    if ($rest_index -ne -1) {
        $rest_args = $args[($rest_index + 1) .. $args.Length]
    }

    Push-Location $project_build
    
    echo ""
    ctest -C $project_config @rest_args

    Pop-Location
}

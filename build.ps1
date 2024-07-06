$project_root = Split-Path -Path $MyInvocation.MyCommand.Path
$project_build = "$project_root/out"
$project_bin = "$project_build/bin"

#build step

cmake -S $project_root -B $project_build -G "NMake Makefiles"

Push-Location $project_build

nmake

Pop-Location

$action = $args[0]

if ($action -eq "run") {
    $rest_args = @()

    $rest_index = [array]::indexof($args, "/args")
    if ($rest_index -ne -1) {
        $rest_args = $args[($rest_index + 1) .. $args.Length]
    }

    echo ""
    echo "& ""$project_bin/general_ll1.exe"" $rest_args"

    & "$project_bin/general_ll1.exe" @rest_args
}

if ($action -eq "test") {
    $rest_args = @()

    $rest_index = [array]::indexof($args, "/args")
    if ($rest_index -ne -1) {
        $rest_args = $args[($rest_index + 1) .. $args.Length]
    }

    Push-Location $project_build
    
    echo ""
    ctest @rest_args

    Pop-Location
}

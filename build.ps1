$PROJECT_DIR = $pwd.Path
$BUILD_DIR = "$pwd\build"

$CL_OPTIONS = "/TC", "/Zi", "/fsanitize=address", "/sdl"
$LINK_OPTIONS = "/out:general_ll1.exe"

$SearchOptions = @{
    Path        = $PROJECT_DIR 
    Filter      = '*.c'
    Recurse     = $true
    ErrorAction = 'SilentlyContinue'
    Force       = $true
}

$SOURCE_FILES = Get-ChildItem @SearchOptions | Where-Object Name -NotMatch '_test.c$' | %{$_.FullName}

echo $SOURCE_FILES

# Push-Location $BULID_DIR
cd $BUILD_DIR

cl.exe $CL_OPTIONS $SOURCE_FILES /link $LINK_OPTIONS

# Pop-Location
cd $PROJECT_DIR


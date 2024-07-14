set outdir=temp\cmake_win64\
cmake . -B %outdir% -G "Visual Studio 17 2022" -A x64 -Dgearmulator_BUILD_FX_PLUGIN=ON -DDSP56300_DEBUGGER=OFF
IF %ERRORLEVEL% NEQ 0 (
	popd 
	exit /B 2
)
pushd %outdir%
cmake --build . --config Release
IF %ERRORLEVEL% NEQ 0 (
	popd 
	exit /B 2
)
cmake -P ../../scripts/pack.cmake
popd

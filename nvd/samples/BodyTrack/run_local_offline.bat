SETLOCAL
SET PATH=%PATH%;..\..\samples\external\opencv\bin;..\..\bin;
BodyTrack.exe --model_path=..\..\bin\models --in=bodytrack.mp4 --offline_mode
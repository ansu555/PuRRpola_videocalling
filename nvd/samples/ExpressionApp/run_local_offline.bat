SETLOCAL
SET PATH=%PATH%;..\..\samples\external\opencv\bin;..\..\bin;
ExpressionApp.exe --model_path=..\..\bin\models --expr_mode=2 --in=..\FaceTrack\FaceTrack.mp4 --out=ExpressionApp_out.mp4
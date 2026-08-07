@echo off
echo AI Meeting Assistant - GitHub Deployment Helper
echo ==============================================
echo.

REM Check if Python is available
python --version >nul 2>nul
if errorlevel 1 (
    echo WARNING: Python not found or not in PATH
    echo Attempting to run basic deployment steps...
    goto :basic_deploy
) else (
    goto :python_deploy
)

:python_deploy
echo Running Python deployment script...
python deploy_to_github.py
pause
exit /b

:basic_deploy
echo.
echo Basic GitHub Deployment Instructions:
echo ====================================
echo.
echo 1. First, make sure Git is installed: https://git-scm.com/downloads
echo.
echo 2. Open Command Prompt in this folder and run:
echo    git init
echo    git add .
echo    git commit -m "Initial commit: AI Meeting Assistant Project"
echo.
echo 3. Create a new repository at: https://github.com/new
echo    - Name: AI-Meeting-Assistant
echo    - Description: ESP32-based meeting recording system with AI processing
echo    - Do NOT initialize with README, .gitignore, or license
echo.
echo 4. Connect and push:
echo    git remote add origin https://github.com/YOUR_USERNAME/AI-Meeting-Assistant.git
echo    git branch -M main
echo    git push -u origin main
echo.
echo 5. Add this to your resume:
echo    GitHub: https://github.com/YOUR_USERNAME/AI-Meeting-Assistant
echo.
pause
@echo off
REM Deployment script for AI Meeting Assistant
echo AI Meeting Assistant - GitHub Deployment
echo.

REM Check if git is installed
where git >nul 2>nul
if errorlevel 1 (
    echo ERROR: Git is not installed or not in PATH
    echo Please install git from: https://git-scm.com/downloads
    pause
    exit /b 1
)

REM Change to project directory
cd /d "%~dp0"

echo Step 1: Add remote repository
echo Run this command after creating repository on GitHub:
echo git remote add origin https://github.com/YOUR_USERNAME/AI-Meeting-Assistant.git
echo.

echo Step 2: Rename branch to main
git branch -M main
echo.

echo Step 3: Push to GitHub
echo Run this command to upload:
echo git push -u origin main
echo.

echo Done! Your project is ready for GitHub.
echo.
echo For your resume, use: https://github.com/YOUR_USERNAME/AI-Meeting-Assistant
pause

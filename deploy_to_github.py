#!/usr/bin/env python3
"""
Script to help deploy AI Meeting Assistant project to GitHub
"""

import os
import subprocess
import sys
from pathlib import Path

def check_git_installed():
    """Check if git is installed and accessible"""
    try:
        subprocess.run(['git', '--version'], check=True, capture_output=True)
        return True
    except (subprocess.CalledProcessError, FileNotFoundError):
        return False

def initialize_git_repo(repo_path):
    """Initialize git repository and make initial commit"""
    os.chdir(repo_path)
    
    print("Initializing git repository...")
    
    # Initialize git
    subprocess.run(['git', 'init'], check=True)
    
    # Add all files
    subprocess.run(['git', 'add', '.'], check=True)
    
    # Initial commit
    commit_message = """Initial commit: AI Meeting Assistant Project

Complete ESP32-based meeting recording system with:
- Audio device firmware with I2S microphone and OLED
- Camera device firmware with ESP32-CAM
- Cloud backend architecture for Azure deployment
- Comprehensive documentation and setup guides
- Hardware specifications and schematics
- Docker configuration for containerized deployment
"""
    subprocess.run(['git', 'commit', '-m', commit_message], check=True)
    
    print(f"Git repository initialized at {repo_path}")

def print_deployment_instructions():
    """Print instructions for deploying to GitHub"""
    print("\n" + "="*60)
    print("DEPLOYMENT INSTRUCTIONS")
    print("="*60)
    
    print("\n1. Create a new repository on GitHub:")
    print("   - Go to https://github.com/new")
    print("   - Repository name: AI-Meeting-Assistant")
    print("   - Description: ESP32-based real-time meeting recording system with AI processing")
    print("   - Choose Public or Private")
    print("   - DO NOT initialize with README, .gitignore, or license")
    
    print("\n2. Connect local repository to GitHub:")
    print("   Run these commands in your project directory:")
    print("   git remote add origin https://github.com/YOUR_USERNAME/AI-Meeting-Assistant.git")
    print("   git branch -M main")
    print("   git push -u origin main")
    
    print("\n3. For resume/LinkedIn, use this GitHub URL:")
    print("   https://github.com/YOUR_USERNAME/AI-Meeting-Assistant")
    
    print("\n4. Project highlights for your resume:")
    print("   - Real-time audio streaming with ESP32 I2S microphone")
    print("   - Multi-camera system with ESP32-CAM modules")
    print("   - Azure cloud integration for STT and AI processing")
    print("   - Docker containerized deployment")
    print("   - Complete hardware-software integration")
    print("   - WiFiManager for easy device configuration")
    print("   - mDNS for local device discovery")
    
    print("\n5. To add this to your existing GitHub account:")
    print("   - First, make sure you're logged in: git config --global user.name 'Your Name'")
    print("   - Set your email: git config --global user.email 'your.email@example.com'")
    print("   - Follow steps 1-3 above")

def check_project_structure(project_path):
    """Verify project structure is complete"""
    required_dirs = ['firmware', 'hardware', 'cloud-backend', 'documentation', 'docker']
    required_files = ['README.md', 'LICENSE', '.gitignore']
    
    print("Checking project structure...")
    
    missing_dirs = []
    for dir_name in required_dirs:
        dir_path = os.path.join(project_path, dir_name)
        if not os.path.exists(dir_path):
            missing_dirs.append(dir_name)
    
    missing_files = []
    for file_name in required_files:
        file_path = os.path.join(project_path, file_name)
        if not os.path.exists(file_path):
            missing_files.append(file_name)
    
    if missing_dirs or missing_files:
        print("WARNING: Project structure incomplete!")
        if missing_dirs:
            print(f"Missing directories: {', '.join(missing_dirs)}")
        if missing_files:
            print(f"Missing files: {', '.join(missing_files)}")
        return False
    
    print("Project structure is complete ✓")
    return True

def main():
    """Main function"""
    project_path = os.path.dirname(os.path.abspath(__file__))
    
    print("AI Meeting Assistant - GitHub Deployment Helper")
    print("="*60)
    
    # Check git installation
    if not check_git_installed():
        print("ERROR: Git is not installed or not in PATH")
        print("Please install git from: https://git-scm.com/downloads")
        sys.exit(1)
    
    # Check project structure
    if not check_project_structure(project_path):
        print("\nPlease fix the missing components before deploying.")
        sys.exit(1)
    
    # Initialize git repository
    try:
        initialize_git_repo(project_path)
    except subprocess.CalledProcessError as e:
        print(f"ERROR: Git initialization failed: {e}")
        sys.exit(1)
    
    # Print deployment instructions
    print_deployment_instructions()
    
    # Summary
    print("\n" + "="*60)
    print("SUMMARY")
    print("="*60)
    print(f"Project location: {project_path}")
    print("Total files created: ~20+ files")
    print("Project size: ~50KB (code) + documentation")
    print("Ready for GitHub deployment!")
    
    # Optional: Create deployment script
    create_deployment_script(project_path)

def create_deployment_script(project_path):
    """Create a batch script for Windows users"""
    script_content = """@echo off
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
"""
    
    script_path = os.path.join(project_path, "deploy_to_github.bat")
    with open(script_path, 'w') as f:
        f.write(script_content)
    
    print(f"\nCreated deployment script: {script_path}")
    print("Run 'deploy_to_github.bat' for step-by-step instructions")

if __name__ == "__main__":
    main()
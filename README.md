<div align="center">

# 🎮 OpenGLFramework v1.0.9

![Version](https://img.shields.io/badge/version-1.0.9-blue)
![C++](https://img.shields.io/badge/C++-23-orange)

A lightweight C++ framework for building 2D games and tools with SDL2 and OpenGL.

</div>

---

## Overview

**OpenGLFrameWork** is a lightweight C++23 abstraction layer built on top of SDL2 and OpenGL. 
It is specifically designed for beginners and hobbyists who want to create 2D games or 
tools without getting lost in the boilerplate code of window management, texture loading, 
or complex rendering pipelines.

---

## 🚀 Key Features

	### 🎨 Customizable UI System
	Create buttons, text boxes, and click boxes with full control over layout, appearance,
	and behavior. UI layouts can be serialized to and loaded from JSON, enabling dynamic
	interface creation and rapid iteration.

	### ⚡ Multiple Rendering Paths
	Supports several rendering modes, each optimized for different use cases:
	- **Standard Rendering** – general-purpose rendering pipeline
	- **Flat Rendering** – simplified rendering for UI and 2D elements
	- **UberShader Rendering** – flexible shader-based pipeline for advanced effects

	### 📁 Built-in File Explorer
	A fully functional file browser with adjustable scrolling, directory navigation,
	and file format filtering, suitable for tools and editors.

	### 🖼️ Texture Manager
	Load entire directories of PNG textures with a single call.
	Supports dynamic texture reloading during runtime.

	### 🔊 Sound Manager
	Stereo audio playback with support for raw sounds and recursive directory loading.
	Designed for simple integration without complex audio setup.

	### 🗺️ Grid & Logic Utilities
	Built-in tools for working with 2D grids, including collision detection,
	Euclidean distance calculations, and spatial logic helpers.

	### 🎬 Scene Management
	Simple scene system for switching between application states such as
	menus, gameplay, editors, or settings.

## 🛠️ Requirements

| Category        | Requirement                           |
|-----------------|---------------------------------------|
| Language        | C++23 or newer                        |
| Graphics        | OpenGL, GLAD, GLM                     |
| Window / Input  | SDL2, SDL2_image                      |
| Audio / Fonts   | SDL2_mixer, SDL2_ttf                  |


## 📌 Status

The framework is under active development and intended primarily for:
- learning projects
- hobby games
- internal tools and editors
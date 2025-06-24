# Simple and Generic Argument Parser (SGAP)

**SGAP** is a lightweight, flexible argument parser designed for general-purpose use. It supports a variety of input types and uses a CSV-based configuration to define flags and their behavior.

## Features

- Parses three types of inputs:
  - **Text**
  - **Numbers** (decimal and hexadecimal)
  - **Booleans**
- Flags are defined via a **CSV file** (`flags.csv`)
- Supports **dependency between flags and a special flag** (e.g., certain flags require the special flag to be present)
- Designed for simplicity and portability

## File Overview

- `flags.csv` – Contains definitions of all supported flags and their configurations
- `tests.py` – Automated test suite using functions defined in the header file

## Usage

1. Define flags and their types in `flags.csv`
2. Compile the project using the command in `command.txt`
3. Run tests using `tests.py` to validate the parser

## Flag Dependencies

Flags can be configured to require other flags (e.g., flag `"s"` must be present for another flag to function). 

## Build

The argument parser is not divided into different files by purpose because of the environment it was required for.
The argument parser can be compiled into 32bit or 64bit excetubales.

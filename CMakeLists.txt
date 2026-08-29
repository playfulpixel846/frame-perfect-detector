name: Build Geode Mod

on:
  push:
  workflow_dispatch:

jobs:
  build:
    runs-on: windows-latest

    steps:
      - name: Checkout
        uses: actions/checkout@v4

      - name: Build
        uses: geode-sdk/build-geode-mod@main
        with:
          target: Win64

#!/bin/bash

# Define the directory name
DIR_NAME="upmem-2025.1.0-Linux-x86_64"

# Check if the directory does not exist
if [ ! -d "$DIR_NAME" ]; then
    # Download the file
    wget http://sdk-releases.upmem.com/2025.1.0/ubuntu_22.04/upmem-2025.1.0-Linux-x86_64.tar.gz

    # Extract the file
    tar -xvzf upmem-2025.1.0-Linux-x86_64.tar.gz

    # Remove the tar.gz file after extraction (optional)
    rm upmem-2025.1.0-Linux-x86_64.tar.gz
fi


# Source the environment script
source ./$DIR_NAME/upmem_env.sh

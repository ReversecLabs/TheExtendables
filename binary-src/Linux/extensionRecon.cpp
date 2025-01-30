#include "./extendables.h"

// Implementation of strcat_s
void *strcat_safe(char *to, size_t maxSize, const char *from) {
  int toSize = strlen(from);
  if (toSize > maxSize) {
    printf("Value is too large");
    exit(1);
    return 0;
  }

  return strcat(to, from);
}

// Function to parse the contents of a nativemessaging manifest file
int nmManifestParser(char *filePath, char *path, int verbose) {

  // Setup vars
  FILE *fileHandle;
  char *outBuf = 0;
  long fileSize = 0;
  char *occuranceAddr = 0;
  char *occuranceEndAddr = 0;

  // Set current directory
  bool success = chdir(path);

  if (verbose == 1) {
    printf("[*] Directory to set - %s\n", path);
  }

  if (success != 0) {
    printf("[-] Failed to set current directory to NativeMessaging manifest. "
           "May cause errors if relative paths are used.\n");
  }

  // Get a handle to the target manifest and get its size
  fileHandle = fopen(filePath, "r");

  // If retrieving the handle fails, return error
  if (fileHandle == NULL) {
    printf("[-] NativeMessaging file does not seem to exist! - %s\n\n",
           filePath);
    return 2;
  }

  // Get length of file
  fseek(fileHandle, 0L, SEEK_END);
  fileSize = ftell(fileHandle);
  // Reset file pointer to start of file
  fseek(fileHandle, 0L, SEEK_SET);

  // If file is too big, return error
  if (fileSize > 1048576) {
    printf("[-] File is too big!\n\n");
    fclose(fileHandle);
    return 2;
  }

  // Allocate a buffer for the file contents and set all bytes to 0
  outBuf = (char *)calloc(1, fileSize);

  // Read the file contents into the buffer
  fread(outBuf, fileSize, 1, fileHandle);
  fclose(fileHandle);

  // Extract the "path" string from the manifest and check if it is writable
  occuranceAddr = strstr(outBuf, "ath\":");
  if (occuranceAddr == NULL) {
    occuranceAddr = strstr(outBuf, "ath\" ");
  }
  if (occuranceAddr != NULL) {
    // Moving the offset forward to skip the characters we know
    occuranceAddr += 7;
    occuranceEndAddr = strstr(occuranceAddr, "\"");

    char *nmHostPath = (char *)calloc(1, 512);

    memcpy(nmHostPath, occuranceAddr, (occuranceEndAddr - occuranceAddr));

    // Try to open file with write permissions to see if it is writable
    FILE *nativeManifestFile = fopen(nmHostPath, "w");
    if (nativeManifestFile != NULL) {
      printf("NativeMessaging host application: %s\n[*] NativeMessaging host "
             "application writable and hijackable!\n\n",
             nmHostPath);
      fclose(nativeManifestFile);
    } else {
      printf("NativeMessaging host application: %s\n[-] NativeMessaging host "
             "application not hijackable!\n\n",
             nmHostPath);
    }
    // Free buffer
    free(nmHostPath);
  }

  else {
    printf("[-] Unable to identify 'path' in NativeMessaging manifest!\n");
  }
  free(outBuf);
  return 0;
}

// Function to parse the contents of an extension manifest file
int manifestParser(char *path) {
  // Setup vars
  FILE *fileHandle;
  char *outBuf = 0;
  long fileSize = 0;
  char *occuranceAddr = (char *)"";

  // Get a handle to the target manifest and get its size
  fileHandle = fopen(path, "r");

  printf("[*] Manifest file path - %s\n", path);

  // If retrieving the handle fails, return error
  if (fileHandle == NULL) {
    printf("[-] Manifest file does not seem to exist! - %s\n\n", path);
    return 2;
  }
  // Get length of file
  fseek(fileHandle, 0L, SEEK_END);
  fileSize = ftell(fileHandle);
  // Reset file pointer to start of file
  fseek(fileHandle, 0L, SEEK_SET);

  // If file is too big, return error
  if (fileSize > 1048576) {
    printf("[-] File is too big!\n\n");
    fclose(fileHandle);
    return 2;
  }

  // Allocate a buffer for the file contents and set all bytes to 0
  outBuf = (char *)calloc(1, fileSize);

  // Read the file contents into the buffer
  fread(outBuf, fileSize - 1, 1, fileHandle);
  fclose(fileHandle);

  // Check if the "nativeMessaging" string is within the file
  occuranceAddr = strstr(outBuf, "\"nativeMessaging\"");

  // If the "nativeMessaging" string is in the file, it may be vulnerable.
  // Return success
  if (occuranceAddr != 0) {
    printf("[*] Potentially vulnerable extension!\nIf NativeMessaging manifest "
           "does not exist, you can create it using the '/persist' feature of "
           "this tool!\nYou will need to retrieve the extension name from the "
           "legitimate NativeMessaging manifest (you may need to install the "
           "NativeMessaging components legitimately on a test machine to get "
           "this)\nOR\nLook through the extension's JavaScript and find what "
           "is passed to 'runtime.connectNative()' in the format "
           "'com..extensionName' for the extensionName.\n\n\n");
    free(outBuf);
    return 0;
  }

  // If string not in the buffer, the extension does not have the correct
  // permissions
  printf("[-] Not vulnerable!\n\n\n");

  free(outBuf);

  return 1;
}

// Function to enumerate values
int enumFiles(char *path, int verbose, int forcelist) {

  // Set up variable
  dirent *dirFile;
  int found = 0;

  // Open up directory to enumerate
  DIR *directory = opendir(path);
  if (directory == NULL) {
    printf("[-] Error opening folder - %s\n[-] Probably because it has not "
           "been configured!\n",
           path);
    return 1;
  }

  // Loop through each value
  while ((dirFile = readdir(directory)) != NULL) {
    // Build up path to manifest
    char *filePath = (char *)calloc(1, 512);
    strcat_safe(filePath, 512, path);
    strcat_safe(filePath, 512, dirFile->d_name);

    // Check if name is "." or "..", as we don't want to look at those
    if ((strcmp(dirFile->d_name, ".") != 0) &&
        (strcmp(dirFile->d_name, "..") != 0)) {
      found++;
      printf("[*] Checking file - %s\n", filePath);
      // Open manifest with write permissions to see if it is writable
      FILE *nativeManifestFile = fopen(filePath, "w");
      if (nativeManifestFile != NULL) {
        // Check to see if NativeMessaging Manifest or managed extension config
        if (forcelist == 0) {
          printf("[+] NativeMessaging manifest is writable and hijackable by "
                 "the current user!\n");
        } else {
          printf("[+] Managed extension config is writable and hijackable by "
                 "the current user!\n");
        }
        fclose(nativeManifestFile);

      } else {
        if (forcelist == 0) {
          printf("[-] NativeMessaging manifest is not writable by the current "
                 "user!\n");
        } else {
          printf("[-] Managed extension config is not writable by the current "
                 "user!\n");
        }
      }

      // Test to see if the nativeMessaging host application is writable if not
      // looking for the NativeMessaging manifest
      if (forcelist == 0) {
        nmManifestParser(filePath, path, verbose);
      }
    }
    // Free buffer
    free(filePath);
  }

  if (found == 0) {
    printf("[-] No files found!\n");
  }

  // Close the key and free the valueVal
  closedir(directory);
  return 0;
}

// Function to list contents of a directory
int dirListing(char *folderPath) {

  printf("\n[+] Enumerating extensions\n\n\n\n");

  // Set up variables
  dirent *dirFile;

  // Open up directory to enumerate
  DIR *directory = opendir(folderPath);

  if (directory == NULL) {
    printf("[-] Error opening folder - %s\n[-] Probably because it has not "
           "been configured!\n",
           folderPath);
    return 1;
  }

  // Loop through each value
  while ((dirFile = readdir(directory)) != NULL) {

    struct stat statbuf;

    // Build up full path for each item in the directory
    char *currPath = (char *)calloc(1, 512);
    strcat_safe(currPath, 512, folderPath);
    strcat_safe(currPath, 512, dirFile->d_name);

    // Check that file exists, is not "." or ".." is a directory and is not
    // named "Temp"
    if ((stat(currPath, &statbuf) == 0) &&
        (strcmp(dirFile->d_name, ".") != 0) &&
        (strcmp(dirFile->d_name, "..") != 0) && (dirFile != NULL) &&
        S_ISDIR(statbuf.st_mode) && (strcmp(dirFile->d_name, "Temp") != 0)) {

      printf("[+] Extension - %s\n", dirFile->d_name);

      // Set up variables
      dirent *dirFileNew;

      // Open up directory to enumerate
      DIR *directoryNew = opendir(currPath);
      if (directoryNew == NULL) {
        printf("[-] Error opening folder - %s\n[-] Probably because it has not "
               "been configured!\n",
               currPath);
        free(currPath);
        return 1;
      }

      // Loop through each file in directory
      while ((dirFileNew = readdir(directoryNew)) != NULL) {

        struct stat statbufNew;
        // Build up full path for each item in the directory
        char *currPathLoop = (char *)calloc(1, 512);
        strcat_safe(currPathLoop, 512, currPath);
        strcat_safe(currPathLoop, 512, "/");
        strcat_safe(currPathLoop, 512, dirFileNew->d_name);

        // Check that file exists, is not "." or ".." is a directory and is not
        // named "Temp"
        if (stat(currPathLoop, &statbufNew) == 0 &&
            (strcmp(dirFileNew->d_name, ".") != 0) &&
            (strcmp(dirFileNew->d_name, "..") != 0) && (dirFileNew != NULL) &&
            S_ISDIR(statbufNew.st_mode) &&
            (strcmp(dirFileNew->d_name, "Temp") != 0)) {
          // Build full path to manifest file
          strcat_safe(currPathLoop, 512, "/manifest.json");
          printf("[+] Path - %s\n", currPathLoop);

          // Call function to parse manifest file
          manifestParser(currPathLoop);
          break;
        }
        // Free buffer
        free(currPathLoop);
      }
      // CLose dir handle
      closedir(directoryNew);
    }
    // Free buffer
    free(currPath);
  }

  // Close dir handle and free buffer
  closedir(directory);

  return 0;
}

// Function to list contents of a directory
int fireFoxDirListing(char *folderPath) {

  // Get first file in dir and handle for enum
  printf("\n[+] Enumerating extensions\n\n\n\n");

  // Set up variables
  dirent *dirFile;

  // Open up directory to enumerate
  DIR *directory = opendir(folderPath);
  if (directory == NULL) {
    printf("[-] Error opening folder - %s\n[-] Probably because it has not "
           "been configured!\n",
           folderPath);
    return 1;
  }

  // Loop through each value
  while ((dirFile = readdir(directory)) != NULL) {

    struct stat statbuf;

    // Build up full path for each item in the directory
    char *currPath = (char *)calloc(1, 512);
    strcat_safe(currPath, 512, folderPath);
    strcat_safe(currPath, 512, dirFile->d_name);

    // Check that file exists, is not "." or ".." and is a directory
    if (stat(currPath, &statbuf) == 0 && (strcmp(dirFile->d_name, ".") != 0) &&
        (strcmp(dirFile->d_name, "..") != 0) && (dirFile != NULL) &&
        S_ISDIR(statbuf.st_mode)) {

      // Build up full path for each item in the directory
      char *currPathLoop = (char *)calloc(1, 512);
      strcat_safe(currPathLoop, 512, currPath);
      strcat_safe(currPathLoop, 512, "/extensions/");

      // Set up variables
      dirent *dirFileNew;

      // Open up directory to enumerate
      DIR *directoryNew = opendir(currPathLoop);
      if (directoryNew != NULL) {

        printf("\n[+] Profile - %s\n", dirFile->d_name);

        printf("[+] Download the following files, unzip them locally and run "
               "'grep -nriF nativeMessaging' in the folder with the extracted "
               "folders.\nIf any extension's manifest.json contains this "
               "string in the permissions, it is potentially vulnerable:\n");

        // Loop through each value
        while ((dirFileNew = readdir(directoryNew)) != NULL) {
          // Build up full path for each item in the directory
          char *currPathLoop2 = (char *)calloc(1, 512);
          strcat_safe(currPathLoop2, 512, currPathLoop);
          strcat_safe(currPathLoop2, 512, dirFileNew->d_name);

          // Check that file is not "." or ".."
          if ((strcmp(dirFileNew->d_name, ".") != 0) &&
              (strcmp(dirFileNew->d_name, "..") != 0)) {
            // Build full path to manifest file
            printf("%s\n", currPathLoop2);
          }
        }

        // Close dir handle
        closedir(directoryNew);
      }
      // Free buffer
      free(currPathLoop);
    }
    // Free buffer
    free(currPath);
  }

  // Close dir handle and free buffer
  closedir(directory);

  return 0;
}

/*
Lists all extensions and if they are vulnerable for chrome and edge
*/
void extensionRecon(int verbose) {

  // Extension paths for each browser
  char *chromeExtensionPath = (char *)calloc(1, 46 + strlen(getenv("HOME")));
  sprintf(chromeExtensionPath, "%s/.config/google-chrome/Default/Extensions/",
          getenv("HOME"));
  char *chromiumExtensionPath = (char *)calloc(1, 46 + strlen(getenv("HOME")));
  sprintf(chromiumExtensionPath, "%s/.config/chromium/Default/Extensions/",
          getenv("HOME"));
  char *edgeExtensionPath = (char *)calloc(1, 47 + strlen(getenv("HOME")));
  sprintf(edgeExtensionPath, "%s/.config/microsoft-edge/Default/Extensions/",
          getenv("HOME"));
  char *firefoxExtensionPath = (char *)calloc(1, 22 + strlen(getenv("HOME")));
  sprintf(firefoxExtensionPath, "%s/.mozilla/firefox/", getenv("HOME"));

  char *chromeNMCUPath = (char *)calloc(1, 48 + strlen(getenv("HOME")));
  sprintf(chromeNMCUPath, "%s/.config/google-chrome/NativeMessagingHosts/",
          getenv("HOME"));
  char *chromeCUManagedPath = (char *)calloc(1, 47 + strlen(getenv("HOME")));
  sprintf(chromeCUManagedPath, "%s/.config/google-chrome/External Extensions/",
          getenv("HOME"));

  char *chromiumNMCUPath = (char *)calloc(1, 48 + strlen(getenv("HOME")));
  sprintf(chromiumNMCUPath, "%s/.config/chromium/NativeMessagingHosts/",
          getenv("HOME"));
  char *chromiumCUManagedPath = (char *)calloc(1, 47 + strlen(getenv("HOME")));
  sprintf(chromiumCUManagedPath, "%s/.config/chromium/External Extensions/",
          getenv("HOME"));

  char *edgeNMCUPath = (char *)calloc(1, 49 + strlen(getenv("HOME")));
  sprintf(edgeNMCUPath, "%s/.config/microsoft-edge/NativeMessagingHosts/",
          getenv("HOME"));
  char *edgeCUManagedPath = (char *)calloc(1, 48 + strlen(getenv("HOME")));
  sprintf(edgeCUManagedPath, "%s/.config/microsoft-edge/External Extensions/",
          getenv("HOME"));

  char *firefoxNMPath = (char *)calloc(1, 36 + strlen(getenv("HOME")));
  sprintf(firefoxNMPath, "%s/.mozilla/native-messaging-hosts/", getenv("HOME"));

  printf("\n\n\n==========\n[+] Chrome\n==========\n\n\n");

  // Check if file exists to see if chrome is installed
  DIR *chromeExists = opendir(chromeExtensionPath);
  // If retrieving the handle fails, return error
  if (chromeExists == NULL) {
    printf("[-] Chrome does not appear to be installed! \n\n");

  } else {
    closedir(chromeExists);
    // Call function to list files in directory
    dirListing(chromeExtensionPath);
    printf("\n[+] Enumerating existing Chrome NativeMessaging Application's "
           "manifest paths "
           "[+]\n[#]-----------------------------------------------------------"
           "---------------[#]\n\n[+] Current User:\n");
    enumFiles(chromeNMCUPath, verbose, 0);
    printf("\n\n[+] All Users:\n");
    enumFiles((char *)"/etc/opt/chrome/native-messaging-hosts/", verbose, 0);
    printf("\n\n[+] Enumerating managed Chrome extensions "
           "[+]\n[#]---------------------------------------[#]\n\n[+] Current "
           "User:\n\n");
    enumFiles(chromeCUManagedPath, verbose, 1);
    printf("\n\n[+] All Users:\n\n");
    enumFiles((char *)"/opt/google/chrome/extensions/", verbose, 1);
    enumFiles((char *)"/usr/share/google-chrome/extensions/", verbose, 1);
  }

  printf("\n\n\n==========\n[+] Chromium\n==========\n\n\n");

  // Check if file exists to see if chrome is installed
  DIR *chromiumExists = opendir(chromiumExtensionPath);
  // If retrieving the handle fails, return error
  if (chromiumExists == NULL) {
    printf("[-] Chromium does not appear to be installed! \n\n");

  } else {
    closedir(chromiumExists);
    // Call function to list files in directory
    dirListing(chromiumExtensionPath);
    printf("\n[+] Enumerating existing Chromium NativeMessaging Application's "
           "manifest paths "
           "[+]\n[#]-----------------------------------------------------------"
           "---------------[#]\n\n[+] Current User:\n");
    enumFiles(chromiumNMCUPath, verbose, 0);
    printf("\n\n[+] All Users:\n");
    enumFiles((char *)"/etc/chromium/native-messaging-hosts/", verbose, 0);
    printf("\n\n[+] Enumerating managed Chromium extensions "
           "[+]\n[#]---------------------------------------[#]\n\n[+] Current "
           "User:\n\n");
    enumFiles(chromiumCUManagedPath, verbose, 1);
    printf("\n\n[+] All Users:\n\n");
    enumFiles((char *)"/usr/share/chromium/extensions/", verbose, 1);
  }

  // printf("\n\n\n[+]-------------------------------------------------------------[+]\n\n\n[+]
  // Edge\n========\n\n\n");
  printf("\n\n\n==========\n[+] Edge\n==========\n\n\n");
  // Check if file exists to see if chrome is installed
  DIR *edgeExists = opendir(edgeExtensionPath);
  // If retrieving the handle fails, return error
  if (edgeExists == NULL) {
    printf("[-] Edge does not appear to be installed! \n\n");

  } else {
    closedir(edgeExists);
    // Call function to list files in directory
    dirListing(edgeExtensionPath);
    printf("\n[+] Enumerating existing Edge NativeMessaging Application's "
           "manifest paths "
           "[+]\n[#]-----------------------------------------------------------"
           "---------------[#]\n\n[+] Current User:\n");
    enumFiles(edgeNMCUPath, verbose, 0);
    printf("\n\n[+] All Users:\n");
    enumFiles((char *)"/etc/opt/edge/native-messaging-hosts/", verbose, 0);
    printf("\n\n[+] Enumerating managed Edge extensions "
           "[+]\n[#]---------------------------------------[#]\n\n[+] Current "
           "User:\n\n");
    enumFiles(edgeCUManagedPath, verbose, 1);
    printf("\n\n[+] All Users:\n\n");
    enumFiles((char *)"/usr/share/microsoft-edge/extensions/", verbose, 1);
  }

  // printf("\n\n\n[+]-------------------------------------------------------------[+]\n\n\n[+]
  // FireFox\n===========\n\n\n");
  printf("\n\n\n===========\n[+] FireFox\n===========\n\n\n");
  DIR *firefoxExists = opendir(firefoxExtensionPath);
  // If retrieving the handle fails, return error
  if (firefoxExists == NULL) {
    printf("[-] Firefox does not appear to be installed! \n\n");

  } else {
    closedir(firefoxExists);
    // Call function to list files in directory
    fireFoxDirListing(firefoxExtensionPath);
    printf("\n\n[+] Enumerating existing FireFox NativeMessaging Application's "
           "manifest paths "
           "[+]\n[#]-----------------------------------------------------------"
           "----------------[#]\n\n[+] Current User:\n\n");
    enumFiles(firefoxNMPath, verbose, 0);
    printf("\n\n[+] All Users:\n");
    enumFiles((char *)"/usr/lib/mozilla/native-messaging-hosts/", verbose, 0);
  }

  // Free buffers
  free(chromeExtensionPath);
  free(chromiumExtensionPath);
  free(edgeExtensionPath);
  free(firefoxExtensionPath);
  free(chromeNMCUPath);
  free(chromeCUManagedPath);
  free(chromiumNMCUPath);
  free(chromiumCUManagedPath);
  free(edgeNMCUPath);
  free(edgeCUManagedPath);
  free(firefoxNMPath);

  return;
}
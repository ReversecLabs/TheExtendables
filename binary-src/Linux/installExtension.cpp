#include "extendables.h"

/*
Installs an extension that is 'managed by the organisation' in a chromium based
browser.

REQUIRED:
extensionId - identifier string for the extension, eg.
aeblfdkhhhdcdjpifhhbdiojplfjncoa

OPTIONAL:
browser - browser to target: chrome, chromium or edge (all must be lowercase),
default - chrome allUsers - Install for a specific user or all users. <USERNAME>
= specified user (case sensitive), user = current user, machine = all users
(only works if admin). Default = machine
*/
void extensionInstall(char *extensionId, char *browser, char *allUsers) {
  // Set up variables and default values
  char *filePath = (char *)calloc(1, 255);
  FILE *manifest;
  dirent *dirFile;

  // Build path for if user is targeted depending on if it is current or custom
  // user specified
  char *edgeCUManagedPath =
      (char *)calloc(1, 54 + strlen(allUsers) + strlen(getenv("HOME") + 5));
  char *chromiumCUManagedPath =
      (char *)calloc(1, 54 + strlen(allUsers) + strlen(getenv("HOME") + 5));

  if (strcmp(allUsers, "user") == 0) {
    sprintf(edgeCUManagedPath, "%s/.config/microsoft-edge/External Extensions/",
            getenv("HOME"));
    sprintf(chromiumCUManagedPath, "%s/.config/chromium/External Extensions/",
            getenv("HOME"));
  } else {
    sprintf(edgeCUManagedPath,
            "/home/%s/.config/microsoft-edge/External Extensions/", allUsers);
    sprintf(chromiumCUManagedPath,
            "/home/%s/.config/chromium/External Extensions/", allUsers);
  }

  // Length check the extensionID
  int extLen = strlen(extensionId);
  if (extLen > 32) {
    printf("ExtensionID was too long!");
    // Free buffers
    free(filePath);
    free(edgeCUManagedPath);
    free(chromiumCUManagedPath);
    return;
  }
  char *valueContent = (char *)calloc(1, extLen + 47);
  // Check which browser is required
  if (strcmp(browser, "chrome") == 0) {
    // Set whether to use the NativeMessaging Application for all users or just
    // the current user
    if (strcmp(allUsers, "machine") == 0) {
      strcat(filePath, "/usr/share/google-chrome/extensions/");
      printf("\n\n\n[*] Installing for all users\n");
    } else {
      printf("\n\n\n[*] Chrome only allows force installing an extension for "
             "all users on Linux!\n");
      // Free buffers
      free(filePath);
      free(edgeCUManagedPath);
      free(chromiumCUManagedPath);
      free(valueContent);
      return;
    }

    // Open up directory to enumerate
    DIR *directory = opendir(filePath);
    if (directory == NULL) {
      // If directory can't be openend, it may not exist, so we'll try and
      // create it
      if (mkdir(filePath, 0705) != 0) {
        if (strcmp(allUsers, "machine") == 0) {
          // If that doesn't work, we'll try and create the parent directory
          // too, incase that doesn't exist
          if (mkdir("/usr/share/google-chrome/", 0705) == 0) {
            // If we can create the parent directory we'll attempt to create the
            // target directory again
            if (mkdir(filePath, 0705) != 0) {
              printf("[-] Error creating directory - %s!\n Do you have "
                     "permissions to write to this location?\n",
                     filePath);
            } else {
              printf("[-] Error creating directory - %s!\n Do you have "
                     "permissions to write to this location?\n",
                     filePath);
            }
          }
        } else {
          printf("[-] Error creating directory - %s!\n Do you have permissions "
                 "to write to this location?\n",
                 filePath);
        }
      }
    }
    // Build up file path
    strcat(filePath, extensionId);
    strcat(filePath, ".json");
    // Create force install manifest in directory
    manifest = fopen(filePath, "w");
    if (manifest == NULL) {
      printf("Error opening - %s, are you sure you have the permissions?\n\n",
             filePath);
      // Free buffers
      free(filePath);
      free(edgeCUManagedPath);
      free(chromiumCUManagedPath);
      free(valueContent);
      return;
    }
    fprintf(manifest, "{ \"external_update_url\": "
                      "\"https://clients2.google.com/service/update2/crx\" }");
    fclose(manifest);
    // Change the permissions to make sure all user's browsers can read (not
    // write) the file
    chmod(filePath, 0705);
    closedir(directory);

  }

  else if (strcmp(browser, "chromium") == 0) {
    // Set whether to use the NativeMessaging Application for all users or just
    // the current user
    if (strcmp(allUsers, "machine") == 0) {
      strcat(filePath, "/usr/share/chromium/extensions/");
      printf("\n\n\n[*] Installing for all users\n");
    } else {

      strcat(filePath, chromiumCUManagedPath);
      if (strcmp(allUsers, "user") == 0) {
        printf("\n\n\n[*] Installing for current user\n");
      } else {
        printf("\n\n\n[*] Installing for user - %s\n", allUsers);
      }
    }

    // Open up directory to enumerate
    DIR *directory = opendir(filePath);
    if (directory == NULL) {
      // If directory can't be openend, it may not exist, so we'll try and
      // create it
      if (mkdir(filePath, 0705) != 0) {
        if (strcmp(allUsers, "machine") == 0) {
          // If that doesn't work, we'll try and create the parent directory
          // too, incase that doesn't exist
          if (mkdir("/usr/share/chromium/", 0705) == 0) {
            // If we can create the parent directory we'll attempt to create the
            // target directory again
            if (mkdir(filePath, 0705) != 0) {
              printf("[-] Error creating directory - %s!\n Do you have "
                     "permissions to write to this location?\n",
                     filePath);
            } else {
              printf("[-] Error creating directory - %s!\n Do you have "
                     "permissions to write to this location?\n",
                     filePath);
            }
          }
        } else {
          printf("[-] Error creating directory - %s!\n Do you have permissions "
                 "to write to this location?\n",
                 filePath);
        }
      }
    }
    // Build up file path
    strcat(filePath, extensionId);
    strcat(filePath, ".json");
    // Create force install manifest in directory
    manifest = fopen(filePath, "w");
    if (manifest == NULL) {
      printf("Error opening - %s, are you sure you have the permissions?\n\n",
             filePath);
      // Free buffers
      free(filePath);
      free(edgeCUManagedPath);
      free(chromiumCUManagedPath);
      free(valueContent);
      return;
    }
    fprintf(manifest, "{ \"external_update_url\": "
                      "\"https://clients2.google.com/service/update2/crx\" }");
    fclose(manifest);
    // Change the permissions to make sure all user's browsers can read (not
    // write) the file
    chmod(filePath, 0705);
    closedir(directory);

  } else if (strcmp(browser, "edge") == 0) {
    // Set whether to use the NativeMessaging Application for all users or just
    // the current user

    if (strcmp(allUsers, "machine") == 0) {
      memcpy(filePath, "/usr/share/microsoft-edge/extensions/", extLen + 47);
      printf("\n\n\n[*] Installing for all users\n");
    } else {

      strcat(filePath, edgeCUManagedPath);
      if (strcmp(allUsers, "user") == 0) {
        printf("\n\n\n[*] Installing for current user\n");
      } else {
        printf("\n\n\n[*] Installing for user - %s\n", allUsers);
      }
    }
    // Open up directory to enumerate
    DIR *directory = opendir(filePath);
    if (directory == NULL) {

      // If directory can't be opened, it may not exist, so we'll try and create
      // it
      if (mkdir(filePath, 0705) != 0) {
        if (strcmp(allUsers, "machine") == 0) {
          // If that doesn't work, we'll try and create the parent directory
          // too, in case that doesn't exist
          if (mkdir("/usr/share/microsoft-edge/", 0705) == 0) {
            // If we can create the parent directory we'll attempt to create the
            // target directory again
            if (mkdir(filePath, 0705) != 0) {
              printf("[-] Error creating directory - %s!\n Do you have "
                     "permissions to write to this location?\n",
                     filePath);
            } else {
              printf("[-] Error creating directory - %s!\n Do you have "
                     "permissions to write to this location?\n",
                     filePath);
            }
          }
        } else {
          printf("[-] Error creating directory - %s!\n Do you have permissions "
                 "to write to this location?\n",
                 filePath);
        }
      }
    }
    // Build up file path
    strcat(filePath, extensionId);
    strcat(filePath, ".json");
    // Create force install manifest in directory
    manifest = fopen(filePath, "w");
    if (manifest == NULL) {
      printf("Error opening - %s, are you sure you have the permissions?\n\n",
             filePath);
      // Free buffers
      free(filePath);
      free(edgeCUManagedPath);
      free(chromiumCUManagedPath);
      free(valueContent);
      return;
    }
    fprintf(manifest,
            "{ \"external_update_url\": "
            "\"https://edge.microsoft.com/extensionwebstorebase/v1/crx\" }");
    fclose(manifest);
    // Change the permissions to make sure all users' browsers can read (not
    // write) the file
    chmod(filePath, 0705);
    closedir(directory);

  } else {
    printf("\n\n\n[-] Browser not recognised! Use Chrome or Edge\n");
    // Free buffers
    free(filePath);
    free(edgeCUManagedPath);
    free(chromiumCUManagedPath);
    free(valueContent);
    return;
  }

  printf("[*] Extension installed for %s successfully at - %s!\n", browser,
         filePath);

  // Free buffers
  free(filePath);
  free(edgeCUManagedPath);
  free(chromiumCUManagedPath);
  free(valueContent);

  return;
}
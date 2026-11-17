# Phone Book — Second-Year Coursework

A C++ contact manager with a console interface and a Qt desktop interface. Create, find, edit, delete, and sort contacts, with support for names, email, multiple phone types, addresses, and birthdays.

## Two interfaces

| Version | Interface | Storage | Dependencies |
| --- | --- | --- | --- |
| [Console](phonebookcli/) | Interactive numbered menu | Local text-format `phonebook.db` | C++17 compiler |
| [Desktop](phonebookgui/) | Qt Widgets dialogs | PostgreSQL with JSON file fallback | Qt Widgets, Qt SQL, C++17 compiler |

[Read the coursework report](phonebookreport.pdf).

## Run the console application

From the repository root, using a C++17-capable compiler:

```sh
mkdir build
g++ -std=c++17 -Wall -Wextra phonebookcli/main.cpp phonebookcli/contact.cpp phonebookcli/checkers.cpp phonebookcli/definitions.cpp phonebookcli/menus.cpp -o build/phonebook
cd build
```

On Windows PowerShell, run `./phonebook.exe`. On Linux or macOS, run `./phonebook`.

Choose a menu number to create, search, edit, delete, or list contacts. Enter `quit` to exit. Running from `build` keeps your local contacts separate from the existing file in `phonebookcli`.

## Run the desktop application

1. Install Qt with a matching C++ compiler kit, Widgets, and SQL support.
2. Open `phonebookgui/phonebookgui.pro` in Qt Creator.
3. Select your desktop kit and use a separate build directory.
4. Build and run the application.

The application attempts PostgreSQL first and falls back to a local JSON file if the connection fails. PostgreSQL mode requires the Qt `QPSQL` driver and its PostgreSQL client libraries. The application prints available SQL drivers on startup.

### Optional PostgreSQL setup

Default connection values are `localhost:5432`, database `phonebook_db`, and user `postgres`; these defaults are defined in `PhoneBookgui.h`.

Create a dedicated empty database and initialize it from the repository root:

```sh
createdb -U postgres phonebook_db
psql -U postgres -d phonebook_db -f phonebookgui/schema.sql
```

**The schema script drops existing contact and phone tables. Use it only for a new database or when deliberately resetting disposable data.**

Set `PHONEBOOK_DB_PASSWORD` in the application's environment before launching it. In Qt Creator, add it under the project's Run Environment settings. The program no longer uses a built-in password.

## Storage notes

- Both interfaces use the filename `phonebook.db` relative to their working directory.
- The console file is a custom text format; the desktop fallback is JSON. They are not interchangeable and should use separate working directories.
- The desktop includes a JSON/PostgreSQL migration dialog.
- Validation follows coursework-specific rules. The database schema applies additional constraints, including a restrictive email pattern; some conventional email addresses will be rejected.

## Repository guide

- `phonebookcli/`: console menus, contact model, validation, and file persistence.
- `phonebookgui/`: Qt dialogs, contact management, PostgreSQL integration, and schema.
- `phonebookreport.pdf`: coursework report.

Build from source for your own platform. The existing `phonebookcli/run.exe` is a previously committed binary, not a verified release. Generated binaries, build folders, and new local database files are excluded from future commits.

## Validation scope

The Qt interface and PostgreSQL workflows require a configured Qt/PostgreSQL environment and have not been validated in this repository improvement pass.

The console application was compiled with GCC using C++17 and warning flags, then checked for successful startup and clean exit. This is a smoke check, not a full functional test suite.

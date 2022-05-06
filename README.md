# adwinGUI
Developer documentation can be found at <https://tecnd.github.io/adwinGUI/>.
## User Guide
### Menu bar
- File
  - Load Parameters - Loads panel from a .pan/.arr pair. Both files must have the same name and be in
  the same directory.
  - Save Parameters - Saves panel to a .pan/.arr pair.
  - Export Channel Config - Deprecated. Writes analog and digital channel settings to a text file.
  - Export Panel - Writes analog and digital cell values to a text file for use in an external program.
  Skips columns with negative time. A column with a time of zero marks the end of a page.
  - Export Panel and Launch Python - Writes analog and digital cell values to a temporary text file and
  opens it in the Python plotter.
  - Exit - Exits the program.
- Settings
  - Analog Settings - Makes changes to analog lines, including channel number, name, units, and voltage
  limits. Must click Allow Changes to enable editing and Set Changes for each line.
  - Digital Settings - Makes changes to digital lines, including channel number and name. Channels 1-32
  refer to channels 1-32 on the first digital I/O card (labeled 1-32), and channels 101-132 refer to
  channels 1-32 on the second digital I/O card (labeled 33-64). Must click Allow Changes to enable
  editing and Set Changes for each line.
  - Reboot ADwin - Reuploads the ADwin program and bootloader to the ADwin.
  - Clear Panel - Resets all cells to zero.
  - Scan Setup - Opens the scan setup panel.
- Edit
  - Note: All of these require a cell in the time table to be selected.
  - Insert Column - Inserts an empty column at the selected position. The last column on the page will
  be lost.
  - Delete Column - Deletes the column at the selected position and moves the following columns to the left.
  - Copy Column - Copies the column at the selected position.
  - Paste Column - Pastes the column to the selected position.
- Preferences
  - Use Compression - When generating ADwin tables, replace consecutive zeros with a negative integer
  representing the number of zeros. Greatly reduces the final size of the ADwin tables, leave on unless
  you have good reason not to.
  - Use Simple Timing - When calculating analog outputs, ignores the timescale parameter and treats the full
  cell as the timescale. Leave on unless you have good reason not to.
  
### Page buttons
- Right-click to rename the page.

### Analog and Digital Tables
- Double-click to change the cell's value. On the analog table, this opens a settings window. On the
digital table, this toggles the cell.
- Right-click -> Copy - Copies the value of the highlighted cell.
- Right-click -> Paste - Pastes the copied value into all highlighted cells.

### Scan Values Table
- Right-click -> Load Values to generate scan values.

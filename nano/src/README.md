- browser.c : The specific file, browser.c, is responsible for implementing the file browser functionality in Nano. This allows users
              to navigate and select files in a directory, either for opening or for other operations. 

- chars.c   : Он содержит различные функции для обработки символов в строках, включая поддержку многобайтовых символов, 
              таких как UTF-8, и работу с символами в разных кодировках.

- colors.s  :  Он реализует функциональность, связанную с использованием цветов в интерфейсе и подсветке синтаксиса.

- cut.c     : managing text operations like deleting, cutting, copying, pasting, and undoing those actions

- definitions.h : specifically for handling various definitions and configurations

- files.c : взаимодействие с файлом

- global.c : этот файл нужно изучить

- help.c   : It is focused on displaying help information for users within nano's interface. The code in the provided segment deals 
             with the construction and display of the help screen.

- history.c : Этот файл является частью программы GNU nano и отвечает за управление историей поиска, замены и позиций курсора. В нем
              реализованы функции для загрузки, сохранения и обновления истории поиска, замены и команды, а также истории позиций курсора 
              при редактировании файлов.

- move.c  : part of the GNU Nano text editor, specifically responsible for managing cursor movement and screen rendering in the editor

- nano.c : нужно изучить 

- prompt.c : focus on handling user input in a status bar prompt. The specific functionality involves moving the cursor within an 
             input field, editing the text, handling shortcuts, and managing user interaction via the status bar.

- search.c : dealing with search and replace functionality, including regular expression support.

- text.c  : specifically the file handling and text manipulation functions. It provides various features for text editing

- utils.c  : specifically seems to deal with utility functions that aid in various operations like handling memory,
             string manipulation, handling file paths, and parsing numbers

- winio.c : Этот файл отвечает за обработку ввода с клавиатуры и работу с буфером клавиш, а также за управление экраном в терминале
            (например, за обновление состояния курсора).

Основные настройки
set nocompatible              " Отключаем совместимость с Vi
filetype plugin indent on     " Включаем определение типа файла, плагины и автоотступы
syntax on                     " Включаем подсветку синтаксиса

" Настройки отступов
set tabstop=4                 " Ширина табуляции в пробелах
set shiftwidth=4              " Ширина сдвига при использовании >> и <<
set expandtab                 " Преобразует табы в пробелы
set smarttab                  " Умное использование табуляции
set autoindent                " Автоматический отступ новой строки

" Настройки поиска
set incsearch                 " Поиск по мере набора
set hlsearch                  " Подсветка результатов поиска
set ignorecase                " Игнорировать регистр при поиске
set smartcase                 " Умное игнорирование регистра (если есть заглавные буквы, регистр учитывается)

" Настройки интерфейса
set number                    " Статическая нумерация строк (не относительная)
set norelativenumber          " Отключаем относительную нумерацию строк
set cursorline                " Подсветка текущей строки
set showmatch                 " Показывать совпадение скобок
set wildmenu                  " Улучшенное автодополнение в командной строке
set laststatus=2              " Всегда показывать строку статуса
set ruler                     " Показывать позицию курсора

" Настройки для C++
autocmd FileType cpp setlocal commentstring=//\ %s " Использовать // для комментариев в C++
autocmd FileType cpp setlocal foldmethod=syntax    " Включаем фолдинг по синтаксису для C++
autocmd FileType cpp setlocal nofoldenable         " Отключаем автоматическое сворачивание функций

" Плагины (требуется установка менеджера плагинов, например, vim-plug)
call plug#begin('~/.vim/plugged')

Plug 'vim-syntastic/syntastic'                     " Проверка синтаксиса
Plug 'preservim/nerdtree'                          " Файловый менеджер
Plug 'vim-airline/vim-airline'                     " Улучшенная строка состояния
Plug 'vim-airline/vim-airline-themes'              " Темы для airline
Plug 'octol/vim-cpp-enhanced-highlight'            " Улучшенная подсветка синтаксиса C++

call plug#end()

" Настройки для syntastic
let g:syntastic_cpp_checkers = ['gcc']
let g:syntastic_cpp_check_header = 1
let g:syntastic_cpp_compiler_options = ' -std=c++17'

" Настройки для NERDTree
map <C-n> :NERDTreeToggle<CR>

" Настройки для airline
let g:airline_theme = 'dark'
let g:airline#extensions#tabline#enabled = 1

" Настройки для улучшенной подсветки C++
let g:cpp_class_scope_highlight = 1
let g:cpp_member_variable_highlight = 1
let g:cpp_class_decl_highlight = 1
let g:cpp_posix_standard = 1
let g:cpp_experimental_template_highlight = 1

" Настройки для компиляции и запуска C++
map <F9> :w <bar> !g++ -std=c++17 -Wall % -o %:r && ./%:r<CR>
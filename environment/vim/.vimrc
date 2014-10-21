"Shift+Tab přepinani mezi okny ve VIMu
map <s-tab> :wincmd w<cr>

" Zapnutí zvýrazňování syntaxe
syntax on
" Automatické odsazování závislé na typu souboru
filetype indent on
set autoindent
set smartindent
set smarttab
set tabstop=4
set shiftwidth=4
set expandtab " prevod tabulatoru na mezery

" Zepta se co se souuborem, kdyz zmeni okno a neulozim
"set confirm
" Zvýrazňování výsledků hledání
"set hls
set bg=dark
"TlistToggle
" Automaticke ukladani pred komandama :next a :make
"set autowrite
" ukazovat prikazovy radek
set showcmd

" F2 - Preview buffer
map <F2> :if &modifiable && !&readonly && &modified <CR> :w<CR> :endif<CR> :bpreviou<cr>

" F3 - Next buffer
map <F3> :if &modifiable && !&readonly && &modified <CR> :w<CR> :endif<CR> :bnext<cr>
set statusline=%<%f%h%m%r%=%b\ 0x%B\ \ %l,%c%V\ %P
set nocp
filetype plugin on
" configure tags - add additional tags here or comment out not-used ones
 set tags+=~/.vim/tags/stm32f4
 set tags+=~/.vim/tags/arm_core
" set tags+=~/.vim/tags/gl
" set tags+=~/.vim/tags/sdl
" set tags+=~/.vim/tags/qt4
" " build tags of your own project with Ctrl-F12
 map <C-F12> :!ctags -R --sort=yes --c++-kinds=+p --fields=+iaS --extra=+q .<CR>
"
" " OmniCppComplete
 let OmniCpp_NamespaceSearch = 1
 let OmniCpp_GlobalScopeSearch = 1
" let OmniCpp_ShowAccess = 1
 let OmniCpp_ShowPrototypeInAbbr = 1 " show function parameters
 let OmniCpp_MayCompleteDot = 1 " autocomplete after .
 let OmniCpp_MayCompleteArrow = 1 " autocomplete after ->
 let OmniCpp_MayCompleteScope = 1 " autocomplete after ::
"let OmniCpp_DefaultNamespaces = ["std", "_GLIBCXX_STD"]
 " automatically open and close the popup menu / preview window
" au CursorMovedI,InsertLeave * if pumvisible() == 0|silent! pclose|endif
" set completeopt=menuone,menu,longest,preview

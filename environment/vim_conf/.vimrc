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
set switchbuf=useopen,usetab

execute pathogen#infect()

let Tlist_Use_Split_Window = 1
"com TT NERDTreeToggle | TlistToggle
com TT NERDTreeToggle 

set nocp
"filetype plugin on
" configure tags - add additional tags here or comment out not-used ones
" " build tags of your own project with Ctrl-F12
set statusline=%F%m%r%h%w\ [FORMAT=%{&ff}]\ [TYPE=%Y]\ [ASCII=\%03.3b]\ [HEX=\%02.2B]\ [POS=%04l,%04v][%p%%]\ [LEN=%L] 

map <C-F11> :!ctags -R --sort=yes --c++-kinds=+p --fields=+iaS --extra=+q .<CR>

autocmd FileType c setlocal tags+=~/.vim/tags/arm
autocmd FileType cpp setlocal tags+=~/.vim/tags/arm
autocmd bufenter * if (winnr("$") == 1 && exists("b:NERDTreeType") && b:NERDTreeType == "primary") | q | endif

" F2 - Preview buffer
map <F2> :if &modifiable && !&readonly && &modified <CR> :w<CR> :endif<CR> :bpreviou<cr>

" F3 - Next buffer
map <F3> :if &modifiable && !&readonly && &modified <CR> :w<CR> :endif<CR> :bnext<cr>

map <F5> :TlistToggle<cr>
map <F4> :NERDTreeToggle<cr>
set tags+=./tags

set formatoptions-=c formatoptions-=r formatoptions-=o


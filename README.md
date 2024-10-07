# Compilando no Windows

<p align="justify">Utilizando do ambiente MSYS2 para a compilação com MinGW, instale os pacotes <code>mingw-w64-glew</code> e <code>mingw-w64-glfw</code> com <code>pacman -S nome_pacote</code>. Tenha certeza que 
  <code>mingw-w64-make</code> e <code>mingw-w64-cmake</code> também estejam instalado (para o CMake pode ser utilizada a versão de Windows nativa).
  Após isso, depois de ter feito um clone desse repositório, crie um pasta 'build' e dê 'cd build' para ela; a partir disso, faça <code>cmake .. -G "Unix Makefiles"</code> e depois <code>make</code>.<br>
  Para compilação em projetos de Visual Studio, siga a parte 'Installation' do site do <a href="https://glew.sourceforge.net/install.html">GLEW</a> e faça o procedimento semelhante para a dll, lib e cabeçalhos
  do <a href="https://www.glfw.org/">GLFW</a>. 
</p>

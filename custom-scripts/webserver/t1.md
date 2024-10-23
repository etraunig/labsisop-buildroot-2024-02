### Laboratório de Sistemas Operacionais (CC)  
**Trabalho Prático**  

### **Roteiro**

#### **1. Objetivos**
1. Expandir o conhecimento sobre construção e suporte de distribuições Linux.
2. Implementar e instalar um servidor Web.
3. Aprender sobre o conteúdo do diretório `/proc`.

#### **2. Descrição**
A implementação deste trabalho consiste na geração de uma distribuição Linux que possua um servidor WEB escrito em Python ou C/C++. Para isso, será necessário adicionar um interpretador Python na distribuição gerada (caso seja utilizado Python), implementar um servidor WEB e escrever uma página HTML simples.

O suporte para a linguagem Python pode ser adicionado através do **menuconfig** do **Buildroot** (submenu _Interpreter languages and scripting_). Contudo, o Python exige uma toolchain que suporte **WCHAR** (um tipo de variável usado para codificação de strings UTF-16). Esse suporte também pode ser adicionado através do **menuconfig**. Será necessária a recompilação total da distribuição (`make clean`, seguido de `make`). 

Caso seja utilizado C/C++, o próprio compilador cruzado criado pelo Buildroot poderá ser utilizado para compilar a aplicação.

O objetivo da página HTML é fornecer informações básicas sobre o funcionamento do sistema (target). Abaixo, segue a lista de informações que devem ser apresentadas na página de maneira dinâmica:

- **Data e hora do sistema**;
- **Uptime** (tempo de funcionamento sem reinicialização do sistema) em segundos;
- **Modelo do processador** e velocidade;
- **Capacidade ocupada do processador** (%);
- **Quantidade de memória RAM** total e usada (MB);
- **Versão do sistema**;
- **Lista de processos em execução** (PID e nome);
- **Lista de unidades de disco**, com **capacidade** total de cada unidade;
- **Lista de dispositivos USB**, com a respectiva **porta** em que estão conectados;
- **Lista de adaptadores de rede**, com o respectivo **endereçamento IP** de cada um.

Todas as informações acima devem ser obtidas obrigatoriamente através do pseudo-filesystem `/proc`.

Algumas dessas informações são dinâmicas (mudam constantemente), assim, a cada vez que o usuário acessar a página, o servidor deve atualizar os dados.

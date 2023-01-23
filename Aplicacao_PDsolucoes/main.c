/*
 * Aplicacao_PDsolucoes.c
 *
 * Created: 16/01/2023 17:47:18
 * Author : André Luis da Silva
 */ 

#include <comum.h>

/* Variáveis externas */
extern char recebeuDado;

/* Struct de dados para usuário */
struct Cadastro{
	
	char nome[12];	// 12 caracteres para nome (endereço 0 até 11)
	char senha[6];	// 6 bytes para a senha de 6 dígitos (endereço 12 até 18)
	char ADM;	// Último byte para definir ADM(1) ou não(0) (endereço 19)
	char porta;	// Define a porta que foi aberta (endereço 20)

}CadastroUsuarios[QUANTIDADE_CADASTROS];

/* Variáveis de 8 bits */
unsigned char g_contDadosMemoria = 0, g_contDadosEeprom = 0, g_flagPortaAtiva = 0;
unsigned char flagPorta1 = 0, flagPorta2 = 0;

/* Protótipo de funções */
void MenuPrincipal( void );
void InicializaStruct( void );
void CadastroUsuario( void );
void ListaUsuario( void );
void ListaEventos( void );
void LiberaSala1( char portaLiberada );

int main(void)
{
	sei();
	ConfigUart();
	ConfigGpio();
	InicializaStruct();
	TimerConfig();
    /* Replace with your application code */

    while (1) 
    {
		MenuPrincipal();	// Chama o menu principal 			
    }
}

void InicializaStruct( void )	
{
	unsigned char contDados, flagFim = 0;
	if( EepromRead(0x00) != 0xFF )  // Se EepromRead(0x00) diferente a 0xFF
	{
		for( unsigned char contCadastros = 0; contCadastros < QUANTIDADE_CADASTROS; contCadastros++ )// Loop para todos os cadastros
		{	
			for( contDados = g_contDadosMemoria * QUANTIDADE_DADOS; 
				 contDados < QUANTIDADE_DADOS * (g_contDadosMemoria + 1);
				 contDados++ )// Loop para os dados cadastrados	
			{
				if( EepromRead( contDados ) == 0xFF )	// Se EepromRead(dadosCadastrados) igual a 0xFF
				{
					flagFim = 1;
					break;// break;
				}			
				else  // Senão
				{
					if( contDados % QUANTIDADE_DADOS < 12 )	// Se contDados é menor que 12, lê o nome armazenado
					{		
						CadastroUsuarios[contCadastros].nome[ contDados % QUANTIDADE_DADOS] = EepromRead( contDados );
					}
					else if( contDados % QUANTIDADE_DADOS < 18 ) // Se contDados é menor que 18, lê a senha
					{
						CadastroUsuarios[contCadastros].senha[ contDados % QUANTIDADE_DADOS-12] = EepromRead( contDados );
					}
					else if( contDados % QUANTIDADE_DADOS < 19 ) // Se contDados menor que 19, lê se é ADM
					{
						CadastroUsuarios[contCadastros].ADM = EepromRead( contDados );
					}
					else   // Senão, armazena o uúltimo byte para a porta acionada
					{
						CadastroUsuarios[contCadastros].porta = EepromRead( contDados );
					}
				}
			}
			if(flagFim)	// Se identificou uma área da memória que não está salva
			{
				break;	// Sai do loop
			}
			g_contDadosMemoria++;	// Incrementa contador de dados global
		}	
	}	
	for( int i = ENDERECO_EVENTOS + (g_contDadosEeprom * QUANTIDADE_DADOS_EVENTOS); i < ENDERECO_EVENTOS + ((g_contDadosEeprom + 1) * QUANTIDADE_DADOS_EVENTOS); i = i + QUANTIDADE_DADOS_EVENTOS)
	{
		if( EepromRead( i ) == 0xFF )	// Se EepromRead(dadosCadastrados) igual a 0xFF
		{
			break;// break;
		}
		g_contDadosEeprom++;
	}
}
	


void MenuPrincipal( void ){

	EscritaTexto("Menu Principal Gerenciamento de Acesso\r", sizeof("Menu Principal Gerenciamento de Acesso\r"));
	EscritaTexto("PD Solucoes\r", sizeof("PD Solucoes\r"));
	EscritaTexto("1 - Cadastro do usuario\r", sizeof("1 - Cadastro do usuário\r"));
	EscritaTexto("2 - Lista dos usuarios cadastrados\r", sizeof("2 - Lista dos usuarios cadastrados\r"));
	EscritaTexto("3 - Lista dos eventos\r", sizeof("3 - Lista dos eventos\r"));
	EscritaTexto("4 - Libera a porta 1\r", sizeof("4 - Libera a porta 1\r"));
	EscritaTexto("5 - Libera a porta 2\r", sizeof("5 - Libera a porta 2\r"));
	EscritaTexto("Escolha o comando (entre 1 e 5): ", sizeof("Escolha o comando (entre 1 e 5): "));
	recebeuDado = 0;
	
	do   // Faça enquanto o número definido não está entre 1 e 5
	{
		if( recebeuDado != 0 )	// Se há algo na serial
		{
			EscritaTexto(&(recebeuDado),1);
			if( recebeuDado < '1' || recebeuDado > '5')	// Se o valor está fora de 1 a 5
			{
				EscritaTexto("Numero errado!. Escolha entre 1 a 5\r", sizeof("Numero errado!. Escolha entre 1 a 5\r"));
				recebeuDado = 0;
			}
		}

	}while( recebeuDado < '1' || recebeuDado > '5');
	PULA_LINHA;	
	switch( recebeuDado )	// Escolha de acordo com o estado definido
	{
		case '1':	// Caso 1 - Cadastro de usuário
			CadastroUsuario();	// Chama a função de cadastro do usuário
		break;
		case '2':	// Caso 2 - Lista de usuários
			ListaUsuario();
		break;
		case '3':	// Caso 3 - Lista de eventos
			ListaEventos();		
		break;
		case '4':	// Caso 4 - Libera a porta 1
			LiberaSala1(1);
		break;
		case '5':	// Caso 5 - Libera a porta 2
			LiberaSala1(2);
		break;
	}
	PULA_LINHA;
}

void CadastroUsuario( void )
{
	unsigned char contTexto = 0, flagEnter = 0;
	static unsigned int enderecoEeprom = 0;
	recebeuDado = 0;
	enderecoEeprom = g_contDadosMemoria;
	EscritaTexto("Menu de Cadastro do Usuario\r", sizeof("Menu de Cadastro do Usuario\r"));	// Escreve "Menu de Cadastro do Usuário"
	EscritaTexto("Digite o nome do usuario (maximo 12 caracteres): \r", sizeof("Digite o nome do usuario (maximo 12 caracteres): \r"));	// Escreve "Digite o nome do usuario (maximo 12 caracteres):
	while( flagEnter != 1 )
	{
		do   // Faça enquanto o número definido não está entre 0 e 9
		{
			if( recebeuDado != 0 )	// Se há algo na serial
			{
				EscritaTexto(&(recebeuDado),1);
				if( recebeuDado >= '0' && recebeuDado <= '9' )	// Se o valor está fora de 1 a 5
				{
					EscritaTexto("Digite apenas letras\r", sizeof("Digite apenas letras\r"));
					recebeuDado = 0;
				}
				else if( recebeuDado != 0 )	// Se há caracter a ser lido
				{
					if( recebeuDado == 0x08 ) // Se backspace
					{
						if( contTexto > 0 )	// Impede que seja menor que 0
						{	
							contTexto--;	// Decrementa contTexto
						}
						else
						{
							contTexto = 0;	// contTexto recebe 0
						}
					}
					else
					{
						CadastroUsuarios[enderecoEeprom].nome[contTexto] = recebeuDado;
						contTexto++;
						if( recebeuDado == '\r')	// Se chegou ao final do texto
						{
							flagEnter = 1;	// Termina a leitura
							break;
						}
					}
					
					recebeuDado = 0;
					if( contTexto == 12 )	// Chegou ao final do número de caracteres
					{
						flagEnter = 1;	// Termina a leitura
						break;
					}
					
				}
			}

		}while( recebeuDado >= '0' && recebeuDado <= '9' );
	}
	EscritaTexto("Nome cadastrado\r", sizeof("Nome cadastrado\r"));	// Escreve "Nome cadastrado"
	EscritaTexto("Entre com a senha (apenas 6 numeros): \r", sizeof("Entre com a senha (apenas 6 numeros): \r"));		// Escreve "Entre com a senha (apenas numeros): "
	flagEnter = 0;
	contTexto = 0;
	recebeuDado = 0;
	while( flagEnter != 1 )	
	{
		do   // Faça enquanto o número definido não está entre 0 e 9
		{
			if( recebeuDado != 0 )	// Se há algo na serial
			{
				EscritaTexto(&(recebeuDado),1);
				if( recebeuDado < '0' || recebeuDado > '9' )	// Se o valor está fora de 0 a 9
				{
					EscritaTexto("Digite apenas numeros\r", sizeof("Digite apenas numeros\r"));
					for(char i = 0; i < contTexto; i++ )
					{
						EscritaTexto(&(CadastroUsuarios[enderecoEeprom].senha[i]), 1);	
					}
			
					recebeuDado = 0;
				}
				else if( recebeuDado != 0 )	// Se há caracter a ser lido
				{
					if( recebeuDado == 0x08 ) // Se backspace
					{
						if( contTexto > 0 )	// Impede que seja menor que 0
						{
							contTexto--;	// Decrementa contTexto
						}
						else
						{
							contTexto = 0;	// contTexto recebe 0
						}
					}
					else
					{
						
						CadastroUsuarios[enderecoEeprom].senha[contTexto] = recebeuDado;
						contTexto++;
					}
					
					recebeuDado = 0;
					if( contTexto == 6 )	// Chegou ao final do número de caracteres
					{
						flagEnter = 1;
						break;
					}
					
				}
			}

		}while( recebeuDado < '0' || recebeuDado > '9' ); // Faça enquanto o caracter é diferente de número
	}	
	PULA_LINHA;
	EscritaTexto("Senha cadastrada\r", sizeof("Senha cadastrada\r"));
	EscritaTexto("Entre com 1 para ADM e 0 para Comum: \r", sizeof("Entre com 1 para ADM e 0 para Comum: \r"));
	flagEnter = 0;
	recebeuDado = 0;
	while( flagEnter != 1 )
	{
		do   // Faça enquanto o número definido não está entre 0 e 9
		{
			if( recebeuDado != 0 )	// Se há algo na serial
			{
				EscritaTexto(&(recebeuDado),1);
				if( recebeuDado < '0' || recebeuDado > '1' )	// Se o valor está fora de 0 a 9
				{
					EscritaTexto("Digite 0 ou 1\r", sizeof("Digite 0 ou 1\r"));
					recebeuDado = 0;
				}
				else if( recebeuDado != 0 )	// Se há caracter a ser lido
				{
					if( recebeuDado == 0x08 ) // Se backspace
					{
						if( contTexto > 0 )	// Impede que seja menor que 0
						{
							contTexto--;	// Decrementa contTexto
						}
						else
						{
							contTexto = 0;	// contTexto recebe 0
						}
					}
					else
					{
						CadastroUsuarios[enderecoEeprom].ADM = recebeuDado;
						flagEnter = 1;
						break;
					}
					recebeuDado = 0;
				}
			}

		}while( recebeuDado < '0' || recebeuDado > '1' ); // Faça enquanto o caracter é diferente de número
	}
	recebeuDado = 0;
	PULA_LINHA;
	char leMemoria = 0;
	for( unsigned  int i = enderecoEeprom * (QUANTIDADE_DADOS); i < (QUANTIDADE_DADOS)*(enderecoEeprom + 1); i++ )
	{
		if( i % (QUANTIDADE_DADOS) < 12 )	// Se o valor menor que 12
		{
			EepromWrite(i ,CadastroUsuarios[enderecoEeprom].nome[ i % (QUANTIDADE_DADOS) ]);	// Armazena os dados 
		}
		else if( i % (QUANTIDADE_DADOS) < 18 )	// Senão se valor menor que 18
		{
			EepromWrite(i,CadastroUsuarios[enderecoEeprom].senha[ i % (QUANTIDADE_DADOS) - 12 ]);	// Armazena a senha
		}
		else if( i % (QUANTIDADE_DADOS) < 19 )    // Senão
		{
			EepromWrite(i,CadastroUsuarios[enderecoEeprom].ADM);	// Armazena se é ADM ou não
		}
		else    // Senão
		{
			EepromWrite(i,CadastroUsuarios[enderecoEeprom].porta + 0x30);	// Armazena se é ADM ou não
		}
	}
	PULA_LINHA;
	enderecoEeprom++;	// Incrementa a posição do endereço
	g_contDadosMemoria = enderecoEeprom;
}

void ListaUsuario( void )
{
	
	unsigned char contUsuario = 0;
	char leMemoria = 0;
	if( EepromRead(0x00) == 0xFF )	// Se o dado na posição 0 é 0xFF
	{
		EscritaTexto("Nao ha funcionarios cadastrados\r", sizeof("Nao ha funcionarios cadastrados\r"));	// Escreve "Não há funcionários cadastrados"
	}
	else	// Senão
	{
		EscritaTexto("Lista de usuarios\r", sizeof("Lista de usuarios\r"));		// Escreve "Lista de usuários\n"
		while( leMemoria != 0xFF )
		{	
			for( char i = 0; i < 12; i++ )	// Para i começando de 0, i < 12, i incrementa de 1. 12 é o número de caracteres para o nome
			{
				leMemoria = EepromRead(contUsuario * (QUANTIDADE_DADOS) + i);	// leMemoria recebe o endereço conUsuario * Quantidade_dados + i
				if( leMemoria == 0xFF || leMemoria == '\r' )	// Se leMemoria igual 0xFF
				{
					break;	// Break;	
				}
				else    // Senão
				{
					EscritaTexto(&(leMemoria),1);	// Escreve a letra	
				}
			}
			contUsuario++;	// Incrementa contUsuario;
			PULA_LINHA;	// Pula linha	
		}
		PULA_LINHA;	// Pula linha
	}
}

void ListaEventos( void )
{
	char contTexto = 0, senha[6], flagEnter = 0;
	recebeuDado = 0;
	if( EepromRead(ENDERECO_EVENTOS) == 0xFF )	// Se o dado na posição 0 é 0xFF
	{
		EscritaTexto("Nao ha eventos cadastrados\r", sizeof("Nao ha eventos cadastrados\r"));	// Escreve "Não há funcionários cadastrados"
		return;
	}
	EscritaTexto("Entre com a senha de admnistrador: ", sizeof("Entre com a senha de admnistrador: "));
	while( flagEnter != 1 )
	{
		do   // Faça enquanto o número definido não está entre 0 e 9
		{
			if( recebeuDado != 0 )	// Se há algo na serial
			{
				EscritaTexto(&(recebeuDado),1);
				if( recebeuDado < '0' || recebeuDado > '9' )	// Se o valor está fora de 0 a 9
				{
					EscritaTexto("Digite apenas numeros\r", sizeof("Digite apenas numeros\r"));
					for(char i = 0; i < contTexto; i++ )
					{
						EscritaTexto(&(senha[i]), 1);
					}
					
					recebeuDado = 0;
				}
				else if( recebeuDado != 0 )	// Se há caracter a ser lido
				{
					if( recebeuDado == 0x08 ) // Se backspace
					{
						if( contTexto > 0 )	// Impede que seja menor que 0
						{
							contTexto--;	// Decrementa contTexto
						}
						else
						{
							contTexto = 0;	// contTexto recebe 0
						}
					}
					else
					{
						senha[contTexto] = recebeuDado;
						contTexto++;
					}
					
					recebeuDado = 0;
					if( contTexto == 6 )	// Chegou ao final do número de caracteres
					{
						flagEnter = 1;
						break;
					}
					
				}
			}

		}while( recebeuDado < '0' || recebeuDado > '9' ); // Faça enquanto o caracter é diferente de número
	}
	PULA_LINHA;
	unsigned char contOk = 0, contPorta = 0, senhaOk = 0, contDadosEventos = 0;
	char leMemoria = 0;
	int i;
	for( char contAdm = 0; contAdm < QUANTIDADE_CADASTROS; contAdm++ )
	{
		
		if( CadastroUsuarios[contAdm].ADM == '1' )
		{
			if( senha[0] == CadastroUsuarios[contAdm].senha[0] &&
				senha[1] == CadastroUsuarios[contAdm].senha[1] &&
				senha[2] == CadastroUsuarios[contAdm].senha[2] &&
				senha[3] == CadastroUsuarios[contAdm].senha[3] &&
				senha[4] == CadastroUsuarios[contAdm].senha[4] &&
				senha[5] == CadastroUsuarios[contAdm].senha[5] )
			{
				contPorta = 0;
				senhaOk = 1;
				for( i = ENDERECO_EVENTOS; i < (ENDERECO_EVENTOS + ((g_contDadosEeprom) * QUANTIDADE_DADOS_EVENTOS)); i++)	// Verifica se em algum dos cadastros tem evento salvo
				{
					leMemoria = EepromRead(i);	// leMemoria recebe o endereço conUsuario * Quantidade_dados + i
					if( leMemoria == 0xFF )	// Se leMemoria igual 0xFF
					{
						break;	// Break;
					}
					else    // Senão
					{
						if( leMemoria == '\r')
						{
							EscritaTexto(" ",sizeof(" "));	// Escreve a letra
						}
						else
						{
							EscritaTexto(&leMemoria,1);
						}
						if( (i - ENDERECO_EVENTOS) % QUANTIDADE_DADOS_EVENTOS == 12 )
						{
							PULA_LINHA;
						}
						
					}
				}
			}
		}
	}
	if( senhaOk == 0 )
	{
		EscritaTexto("Senha incorreta!\r", sizeof("Senha incorreta!\r"));
	}
}

/*void ListaEventos( void )
{
	char contTexto = 0, senha[6], flagEnter = 0;
	recebeuDado = 0;
	if( EepromRead(0x00) == 0xFF )	// Se o dado na posição 0 é 0xFF
	{
		EscritaTexto("Nao ha funcionarios cadastrados\r", sizeof("Nao ha funcionarios cadastrados\r"));	// Escreve "Não há funcionários cadastrados"
		return;
	}
	EscritaTexto("Entre com a senha de admnistrador: ", sizeof("Entre com a senha de admnistrador: "));
	while( flagEnter != 1 )
	{
		do   // Faça enquanto o número definido não está entre 0 e 9
		{
			if( recebeuDado != 0 )	// Se há algo na serial
			{
				EscritaTexto(&(recebeuDado),1);
				if( recebeuDado < '0' || recebeuDado > '9' )	// Se o valor está fora de 0 a 9
				{
					EscritaTexto("Digite apenas numeros\r", sizeof("Digite apenas numeros\r"));
					for(char i = 0; i < contTexto; i++ )
					{
						EscritaTexto(&(senha[i]), 1);
					}
						
					recebeuDado = 0;
				}
				else if( recebeuDado != 0 )	// Se há caracter a ser lido
				{
					if( recebeuDado == 0x08 ) // Se backspace
					{
						if( contTexto > 0 )	// Impede que seja menor que 0
						{
							contTexto--;	// Decrementa contTexto
						}
						else
						{
							contTexto = 0;	// contTexto recebe 0
						}
					}
					else
					{
						senha[contTexto] = recebeuDado;
						contTexto++;
					}
						
					recebeuDado = 0;
					if( contTexto == 6 )	// Chegou ao final do número de caracteres
					{
						flagEnter = 1;
						break;
					}
						
				}
			}

		}while( recebeuDado < '0' || recebeuDado > '9' ); // Faça enquanto o caracter é diferente de número
	}
	PULA_LINHA;
	unsigned char contOk = 0, contPorta = 0, senhaOk = 0;
	char leMemoria;
	for( char contAdm = 0; contAdm < QUANTIDADE_CADASTROS; contAdm++ )
	{

		if( CadastroUsuarios[contAdm].ADM == '1' )
		{
			if( senha[0] == CadastroUsuarios[contAdm].senha[0] &&
				senha[1] == CadastroUsuarios[contAdm].senha[1] && 
				senha[2] == CadastroUsuarios[contAdm].senha[2] && 
				senha[3] == CadastroUsuarios[contAdm].senha[3] && 
				senha[4] == CadastroUsuarios[contAdm].senha[4] && 
				senha[5] == CadastroUsuarios[contAdm].senha[5] )
				{
					contPorta = 0;
					senhaOk = 1;
					for(char i = 0; i < QUANTIDADE_CADASTROS; i++)	// Verifica se em algum dos cadastros tem evento salvo
					{
						if(CadastroUsuarios[i].porta == '1' || CadastroUsuarios[i].porta == '2' )
						{
							for( char j = 0; j < 12; j++ )	// Para i começando de 0, i < 12, i incrementa de 1. 12 é o número de caracteres para o nome
							{
								leMemoria = CadastroUsuarios[i].nome[j];	// leMemoria recebe o endereço conUsuario * Quantidade_dados + i
								if( leMemoria == 0xFF || leMemoria == '\r' )	// Se leMemoria igual 0xFF
								{
									break;	// Break;
								}
								else    // Senão
								{
									EscritaTexto(&(leMemoria),1);	// Escreve a letra
								}
							}
							EscritaTexto(" ",sizeof(" "));
							EscritaTexto(&CadastroUsuarios[i].porta,sizeof(CadastroUsuarios[i].porta));
							PULA_LINHA;
						}
						else
						{
							contPorta++;
						}
					}
					if( contPorta == QUANTIDADE_CADASTROS )
					{
						EscritaTexto("Não ha eventos cadastrados\r", sizeof("Não há eventos cadastrados\r"));
					}
				}
		}
	}
	if( senhaOk == 0 )
	{
		EscritaTexto("Senha incorreta!\r", sizeof("Senha incorreta!\r"));
	}
}*/


void LiberaSala1( char portaLiberada )
{
	char contTexto = 0, senha[6], flagEnter = 0;
	recebeuDado = 0;
	if( EepromRead(0x00) == 0xFF )	// Se o dado na posição 0 é 0xFF
	{
		EscritaTexto("Nao ha funcionarios cadastrados\r", sizeof("Nao ha funcionarios cadastrados\r"));	// Escreve "Não há funcionários cadastrados"
		return;
	}
	EscritaTexto("Entre com a senha de admnistrador: ", sizeof("Entre com a senha de admnistrador: "));
	while( flagEnter != 1 )
	{
		do   // Faça enquanto o número definido não está entre 0 e 9
		{
			if( recebeuDado != 0 )	// Se há algo na serial
			{
				EscritaTexto(&(recebeuDado),1);
				if( recebeuDado < '0' || recebeuDado > '9' )	// Se o valor está fora de 0 a 9
				{
					EscritaTexto("Digite apenas numeros\r", sizeof("Digite apenas numeros\r"));
					for(char i = 0; i < contTexto; i++ )
					{
						EscritaTexto(&(senha[i]), 1);
					}
						
					recebeuDado = 0;
				}
				else if( recebeuDado != 0 )	// Se há caracter a ser lido
				{
					if( recebeuDado == 0x08 ) // Se backspace
					{
						if( contTexto > 0 )	// Impede que seja menor que 0
						{
							contTexto--;	// Decrementa contTexto
						}
						else
						{
							contTexto = 0;	// contTexto recebe 0
						}
					}
					else
					{
						senha[contTexto] = recebeuDado;
						contTexto++;
					}
						
					recebeuDado = 0;
					if( contTexto == 6 )	// Chegou ao final do número de caracteres
					{
						flagEnter = 1;
						break;
					}
						
				}
			}

		}while( recebeuDado < '0' || recebeuDado > '9' ); // Faça enquanto o caracter é diferente de número
	}
	PULA_LINHA;
	unsigned char contOk = 0, contPorta = 0, senhaOk = 0;
	char leMemoria;
	for( char contAdm = 0; contAdm < QUANTIDADE_CADASTROS; contAdm++ )
	{

		if( senha[0] == CadastroUsuarios[contAdm].senha[0] &&
			senha[1] == CadastroUsuarios[contAdm].senha[1] &&
			senha[2] == CadastroUsuarios[contAdm].senha[2] &&
			senha[3] == CadastroUsuarios[contAdm].senha[3] &&
			senha[4] == CadastroUsuarios[contAdm].senha[4] &&
			senha[5] == CadastroUsuarios[contAdm].senha[5] )
		{
			senhaOk = 1;
			g_flagPortaAtiva = 1;
			char i;
			for( i = 0; i < 12; i++ )	// Escreve o nome no endereço dos eventos
			{
				EepromWrite(ENDERECO_EVENTOS + (g_contDadosEeprom * QUANTIDADE_DADOS_EVENTOS) + i, CadastroUsuarios[contAdm].nome[i]);
			}
			if( portaLiberada == 1 )	// Se porta for 1
			{
				/*
				ENDERECO_EVENTOS + (g_contDadosEeprom * QUANTIDADE_DADOS_EVENTOS) = 600 + g_contDadosEeprom * 13
				*/
				flagPorta1 = 1;
				EepromWrite(ENDERECO_EVENTOS + (g_contDadosEeprom * QUANTIDADE_DADOS_EVENTOS) + i, '1');	// Escreve o número '1' na memória
				LED1_ON;
			}
			else if( portaLiberada == 2 )
			{
				/*
				ENDERECO_EVENTOS + (g_contDadosEeprom * QUANTIDADE_DADOS_EVENTOS) = 600 + g_contDadosEeprom * 13
				*/
				flagPorta2 = 1;
				EepromWrite(ENDERECO_EVENTOS + (g_contDadosEeprom * QUANTIDADE_DADOS_EVENTOS) + i, '2');	// Escreve o número '2' na memória
				LED2_ON;
			}
			g_contDadosEeprom++;	// Incrementa o contador de dados da Eeprom
			EscritaTexto("Porta liberada!\r", sizeof("Porta liberada!\r"));	// Escreve "Porta Liberada"
		}
	}
	if( senhaOk == 0 )	// Se não tem senha
	{
		EscritaTexto("Senha incorreta!\r", sizeof("Senha incorreta!\r"));	// Escreve "Senha incorreta!"
	}
}

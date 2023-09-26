//#define EIGENLIB			// uncomment to use Eigen linear algebra library

#include "fun_head_fast.h"

// do not add Equations in this area

MODELBEGIN

// insert your equations here, ONLY between the MODELBEGIN and MODELEND words

// Exercise 3C

EQUATION("Exit") // Retirar a firma que tem o market-share menor que 0.01

v[1] = COUNT("Firma"); // inserindo este comando, garante que apenas uma firma permanece no modelo depois de retirar as firmas nos ciclos. 
if(v[1]>1)
{
v[0] = V("Market_share"); // 
v[2] = V("mi"); // mi é o parâmetro market-share
v[3] = v[0] < v[2];
cur = SEARCH_CND("Market_share", v[3]); 
DELETE( cur );
}
RESULT(0)


EQUATION("Entry")  // Adicionando novas firmas ao objeto SECTOR
v[1] = V("switch_entry"); // 0 = no entry, 1 = max_market-share, 2 = random_market-share
if(v[1]==1)
{
v[0] = MAX("Market_share");
cur = SEARCH_CND("Market_share", v[0]);
cur2 = ADDOBJ_EX("FIRMA", cur);
WRITES(cur2, "mi", v[0]);
}

if(v[1]==2)
{
cur = RNDDRAW_FAIR("FIRMA"); // Insere uma firma aleatoriamente
ADDOBJ_EX("FIRMA", cur);
}

RESULT(0)

EQUATION("HHI") // Inverse HHI variable
/*
HHI = 1 / (Market_share)^2
*/

CYCLE(cur, "SECTOR")
{ 
	v[0] = VS(cur, "Market_share");
	v[1] = pow(v[1], 2);
	v[2] = 1 / v[1];
}
RESULT(v[2])
 


// Exercise 3B

EQUATION("Sum_up_ms") // Somando market-share utilizando o cycle
v[0]=0;
CYCLE(cur, "FIRMA")  
{
	v[1]=VS(cur, "Market_share");
	v[0] = v[0] + v[1];
	v[2] = v[0] + 1;
}
RESULT(v[2])

// Normalization for consistency
EQUATION("Sum_ms")
RESULT(SUM("Market_share"))

EQUATION("Consistency") // Variável Teste para saber se o somatório do market share = 1
v[0] = V("Sum_ms");
if(v[0]==1)
	v[0] = v[0];
	else
	v[0]==0;
RESULT(v[0])
	

EQUATION("Sum_ms_consist")
v[0] = 0;
v[4] = V("Sum_ms");
CYCLE(cur, "FIRMA");
{
	v[1]=VS(cur, "Market_share");
	v[0] = v[0] + v[1];
	v[2] = v[0] + 1;
}
{ 
	v[3] = v[1] / v[4];
}
RESULT(v[3])

// Sum-up after the normalization procedure
EQUATION("Sum_up_ms")
v[0]= 0;
v[3] = V("Sum_ms_consist"); 
CYCLE(cur, "FIRMA");  
{
	v[1]=VS(cur, "Market_share");
	v[0] = v[0] + v[1];
	v[2] = v[0] + 1;
	WRITES(cur, "Sum_ms_consist", v[3]); // ta errado isso aqui
	
}
RESULT(v[2])



// Exercise 3A

EQUATION("Market_share") // Market share based on competitiveness index

/*
Market_share_t = Market_share_t-1 ( 1 + mi (Compet_index - 1)) 
*/

v[0] = VL("Market_share", 1);
v[1] = V("mi"); //  market share adjustment parameter 0 < m_s < 1
v[2] = V("Compet_index");
v[3] = v[0] * ( 1 + v[1] * (v[2] - 1));

RESULT(v[3])


EQUATION("Compet_index")
/*
Compet_index = q_t^e / p_t^e
*/

v[0] = V("Qualityfirm");
v[1] = V("Price");
v[2] = V("elast_q"); // quality elasticity parameter
v[3] = V("elast_p"); // price elasticity parameter
v[4] = pow(v[0],v[2]);
v[5] = pow(v[1],v[3]);
{
v[6]= v[4]/v[5];
	if(v[5]=0)
	v[6]= 0;
	else
	v[6]=v[6];
}

RESULT(v[6])

EQUATION("Qualityfirm") 
/*
Qualityfirm_t = Qualityfirm_t-1 + c_t 
c_t ~ U(q_min, q_max)  
*/

v[0] = VL("Qualityfirm",1);
v[1] = V("c");
v[2] = v[0] + v[1];
RESULT(v[2])


EQUATION ("c")
v[0] = V("min");
v[1] = V("max");
v[2] = uniform(v[0], v[1]);
RESULT(v[2])



// Exercise 2
EQUATION("Max_Price") // Calcular o preço máx do setor usando o cycle
v[0]=0;
CYCLE(cur, "FIRMA")  // o cur aqui indica que quero fazer o cycle no objeto FIRM
{
	v[1]=VS(cur, "Price");
	if(v[1]>v[0])  // Garante que eu estarei selecionando sempre o maior valor.
	v[0] = v[1];  // Sempre que o argumento do if tiverem mais de uma linha, deve-se inserir o colchetes. 
	else
	v[0] = v[0];
}

RESULT(v[0])


EQUATION("Avg_Price") // Calculando o preço médio utilizando o cycle

v[0] = 0;
v[2] = 0;
CYCLE(cur, "FIRMA")
{
	v[1] = VS(cur, "Price");
	v[0] = v[0] + v[1];
	v[2] = v[2] + 1;
	
}
{
v[3] = v[0]/v[2];
}
RESULT(v[3])


// Exercício 1 - Simple Price Model in Kaleckian spirit

EQUATION("Price")
/*
p_t = theta(Desired_price) + (1 - theta)avg_p_t-1

p_t -> preço da firma no período t
theta -> grau de monopólio
Desired_price -> preço desejado
avg_p_t-1 -> preço médio da firma no período anterior
*/

v[0] = V("theta");
v[1] = V("Desired_price");
v[2] = AVEL("Price", 1);
v[3] = (v[0] * v[1] + (1 - v[0]) * v[2]);

RESULT(v[3])

EQUATION("Desired_price")
v[0] = V("markup");
v[1] = V("Cost");
v[2] = (1 + v[0])*v[1];
RESULT(v[2])

EQUATION("Cost")
v[0] = V("avg");
v[1] = V("sd");
v[2] = norm(v[0], v[1]);
RESULT(v[2])














MODELEND

// do not add Equations in this area

void close_sim( void )
{
	// close simulation special commands go here
}

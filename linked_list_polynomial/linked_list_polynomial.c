#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define  _CRT_SECURE_NO_WARNINGS
#define BUFFER_SIZE 50
#define MAX_POLYS 10

typedef struct term {
	int expo; //������ ���� �ƴ� ����
	int coef;

	struct term *next;
} Term;

typedef struct polynomial {
	char name;
	Term *first;
	int size;

}Polynomial;

Polynomial *polys[MAX_POLYS];
int  n = 0;

//function
Term *create_term_instance() { //���׽� �ʱ�ȭ
	Term *t = (Term *)malloc(sizeof(Term));

	t->coef = 0;
	t->expo = 0;

	return t;
}

Polynomial *create_polynomial_instance(char name) { //���׽� �ʱ�ȭ
	Polynomial *ptr_poly = (Polynomial *)malloc(sizeof(Polynomial));

	ptr_poly->name = name;
	ptr_poly->size = 0;
	ptr_poly->first = NULL;

	return ptr_poly;
}

void add_term(int c, int e, Polynomial *poly);
int eval(Polynomial *poly, int x);
int eval_term(Term *term, int x);
void print_poly(Polynomial *p);
void print_term(Term *pTerm);
void process_command();
void handle_print(char name);
void handle_calc(char name, char *x_str);
void handle_definition(char *expression);
Polynomial *create_by_add_two_polynomial(char name, char f, char g);
Polynomial *create_by_parse_polynomial(char name, char *body);
Term *parse_and_add_term(char *expr, int begin, int end, Polynomial *p_poly);
void destroy_polynomial(Polynomial *ptr_poly);
void insert_polynomial(Polynomial *ptr_poly);
void erase_blanks(char *expression);
int read_line(FILE *fp, char* str, int limit);



int main() {
	process_command();
}



void add_term(int c, int e, Polynomial *poly) { //�ϳ��� �� �߰�
	//1. �߰��Ϸ��� �װ� ���� �������� �ִ� ���, ���� ���� ����� ����
	//2. �׷��� ������, ���ο� �� ����
	if (c == 0)
		return;

	Term *p = poly->first, *q = NULL;

	while (p != NULL && p->expo > e) {
		q = p;
		p = p->next;
	}
	if (p != NULL && p->expo == e) { //���� ���� ���� ������
		p->coef += c;
		if (p->coef == 0) { //���ļ� 0�� �Ǵ� ���
			if (q == NULL)
				poly->first = p->next;
			else
				q->next = p->next;
			poly->size--;
			free(p);
		}
		return;
	}
	//�� ���׽� �����ֱ�
	Term *term = create_term_instance();
	term->coef = c;
	term->expo = e;

	if (q == NULL) {
		term->next = poly->first;
		poly->first = term;
	}
	else {
		term->next = p;
		q->next = term;
	}
	poly->size++;
}

//���׽� �� ��� �Լ�(�Լ� �����ε�)
int eval(Polynomial *poly, int x) {
	int result = 0;
	Term *t = poly->first;
	while (t != NULL) {
		result += eval_term(t, x);
		t = t->next;
	}
	return result;
}
//���׽� �� ��� �Լ�(�Լ� �����ε�)
int eval_term(Term *term, int x) {
	int result = term->coef;
	for (int i = 0; i < term->expo; i++) {
		result *= x;
	}
	return result;
}
//���׽� ���
void print_poly(Polynomial *p) {
	printf("%c=", p->name);
	Term *t = p->first;
	while (t != NULL) {
		if (t->coef > 0 && t != p->first)
			printf("+");
		else if (t->coef < 0 && t != p->first)
			printf("-");

		print_term(t);

		t = t->next;
	}
	printf("\n");
}
//���׽� ���
void print_term(Term *pTerm) {
	if (pTerm->expo == 0) {
		printf("%d", abs(pTerm->coef));
		return;
	}
	if (pTerm->expo == 1 && pTerm->coef == 1) {
		printf("x");
		return;
	}
	if (pTerm->expo != 1 && pTerm->coef == 1) {
		printf("x^%d", pTerm->expo);
		return;
	}
	if(pTerm->expo == 1 && pTerm->coef != 1) {
		printf("%dx", abs(pTerm->coef));
		return;
	}
	
	printf("%dx^%d", abs(pTerm->coef), pTerm->expo);
}
//������ main ����
void process_command() {
	char command_line[BUFFER_SIZE];
	char copied[BUFFER_SIZE];
	char *command, *arg1, *arg2;

	while (1) {
		printf("$ ");
		if (read_line(stdin, command_line, BUFFER_SIZE) <= 0)
			continue;

		strcpy(copied, command_line);
		command = strtok(command_line, " ");

		if (!strcmp(command, "print")) {
			arg1 = strtok(NULL, " ");

			if (arg1 == NULL) {
				printf("Invalide arguments.\n");
				continue;
			}
			handle_print(arg1[0]); //������ ���ĸ��� �� ���ڴϱ� arg1[0] ����
		}
		else if (!strcmp(command, "calc")) {
			arg1 = strtok(NULL, " ");
			if (arg1 == NULL) {
				printf("Invalide arguments.\n");
				continue;
			}
			arg2 = strtok(NULL, " ");
			if (arg2 == NULL) {
				printf("Invalide arguments.\n");
				continue;
			}
			handle_calc(arg1[0], arg2);
		}
		else if (!strcmp(command, "exit"))
			break;
		else
			handle_definition(copied); //���׽� �Է¹ް� ����
	}
}
// name�̸� ���� Ư�� ���׽� ���
void handle_print(char name) {
	for (int i = 0; i < n; i++) {
		if (polys[i]->name == name) {
			print_poly(polys[i]);
			return;
		}
	}
}
//Ư�� ���׽Ŀ� ���� �ְ� ���
void handle_calc(char name, char *x_str) {
	int num = atoi(x_str);
	int result;

	for (int i = 0; i < n; i++) {
		if (polys[i]->name == name) {
			result = eval(polys[i], num);
			printf("%d\n", result);
			return;
		}
	}

	printf("Can't find %c", name);
}
//�� ���׽� ���� ����, ���ο� ���׽� ���� 
void handle_definition(char *expression) {
	erase_blanks(expression);

	char *f_name = strtok(expression, "=");
	if (f_name == NULL || strlen(f_name) != 1) {
		printf("Unsupported command.");
		return;
	}

	char *exp_body = strtok(NULL, "=");
	if (exp_body == NULL || strlen(exp_body) <= 0) {
		printf("Invalid expression format.--");
		return;
	}

	//�� ���׽� ���� ����
	if (exp_body[0] >= 'a' && exp_body[0] <= 'z' && exp_body[0] != 'x') {
		char *former = strtok(exp_body, "+");
		if (former == NULL || strlen(former) != 1) {
			printf("Invalid expression format");
			return;
		}
		char *later = strtok(NULL, "+");
		if (later == NULL || strlen(later) != 1) {
			printf("Invalid expression format");
			return;
		}
		Polynomial *pol = create_by_add_two_polynomial(f_name[0], former[0], later[0]);

		if (pol == NULL) {
			printf("Invalid expression format");
			return;
		}
		insert_polynomial(pol);
	}
	//���ο� ���׽� ����
	else {
		Polynomial *pol = create_by_parse_polynomial(f_name[0], exp_body);
		if (pol == NULL) {
			printf("Invalid expression format");
			return;
		}
		insert_polynomial(pol);
	}
}
//�� ���׽��� ���� ���� ���ο� ���׽��� ����
Polynomial *create_by_add_two_polynomial(char name, char f, char g) {
	Polynomial *new_poly;

	Term *former_term = NULL;
	Term *latter_term = NULL;

	int former;
	int latter;

	for (int i = 0; i < n; i++) {
		if (polys[i]->name == f) {
			former = i;
			former_term = polys[i]->first;
		}
		else if (polys[i]->name == g) {
			latter = i;
			latter_term = polys[i]->first;
		}
	}

	if (former_term == NULL || latter_term == NULL)
		return NULL;

	new_poly = create_polynomial_instance(name);

	for (int i = 0; i < polys[former]->size; i++) {
		add_term(former_term->coef, former_term->expo, new_poly);
		former_term = former_term->next;
	}
	for (int i = 0; i < polys[latter]->size; i++) {
		add_term(latter_term->coef, latter_term->expo, new_poly);
		latter_term = latter_term->next;
	}

	return new_poly;
}
//���׽��� �а� �м��ؼ� tokenize�ϴ� �Լ�
Polynomial *create_by_parse_polynomial(char name, char *body) {
	Polynomial *ptr_poly = create_polynomial_instance(name);

	int i = 0, begin_term = 0;

	while (i < strlen(body)) {
		if (body[i] == '+' || body[i] == '-')
			i++;
		while (i < strlen(body) && body[i] != '+' && body[i] != '-') //�� ���� ���� ������ ����
			i++;
		int result = parse_and_add_term(body, begin_term, i, ptr_poly);

		if (result == 0) { //�߸��� ���� ���, ����� �ִ� ���׽� ��ü�� destroy�Ѵ�.
			destroy_polynomial(ptr_poly);
			return NULL;
		}
		begin_term = i;
	}
	return ptr_poly;
}

Term *parse_and_add_term(char *expr, int begin, int end, Polynomial *p_poly) {
	int i = begin;
	int sign_coef = 1, coef = 0, expo = 1;

	//��ȣ ����
	if (expr[i] == '+')
		i++;
	else if (expr[i] == '-') {
		sign_coef = -1;
		i++;
	}
	//��ȣ + �ڸ����� ���� ���ڿ��� ����� ���� ���·� ġȯ�ϴ� ����
	while (i < end && expr[i] >= '0' && expr[i] <= '9') {
		coef = coef * 10 + (int)(expr[i] - '0');
		i++;
	}
	if (coef == 0) //����� 1, -1�� ��� �����Ǵ� ��츦 ���
		coef = sign_coef;
	else
		coef *= sign_coef;
	//�����
	if (i >= end) {
		add_term(coef, 0, p_poly);
		return 1;
	}
	//��� ������ x�� �ƴ� �ٸ� ���ڰ� ���� �ȵ�
	if (expr[i] != 'x')
		return 0;
	i++;
	//��� ������ x�� ������ ���ڿ��� ������ 1�����̶�� �ǹ�
	if (i >= end) {
		add_term(coef, 1, p_poly);
		return 1;
	}
	//x������ �ݵ�� ^�� �;���
	if (expr[i] != '^')
		return 0;
	i++;
	//���� �κ� �б�
	expo = 0;
	while (i < end && expr[i] >= '0' && expr[i] <= '9') {
		expo = expo * 10 + (int)(expr[i] - '0');
		i++;
	}

	add_term(coef, expo, p_poly);
	return 1;
}

void destroy_polynomial(Polynomial *ptr_poly) {
	if (ptr_poly == NULL)
		return;

	Term *t = ptr_poly->first, *tmp;
	//���׽��� ��� �� free
	while (t != NULL) {
		tmp = t;
		t = t->next;
		free(tmp);
	}
	free(ptr_poly);
}

//���׽��� �����Ѵ�
void insert_polynomial(Polynomial *ptr_poly) {
	for (int i = 0; i < n; i++) {
		if (polys[i]->name == ptr_poly->name) {
			destroy_polynomial(polys[i]); //���׽��� ��� �� ���� ���׽� ��ü free
			polys[i] = ptr_poly;
			return;
		}
	}
	polys[n++] = ptr_poly;
}

void erase_blanks(char *expression) {
	int i = 0;
	char tmp[BUFFER_SIZE];

	strcpy(tmp, expression);

	for (int x = 0; tmp[x] != '\0'; x++) {
		if (tmp[x] != ' ') {
			expression[i++] = tmp[x];
		}
	}
	expression[i] = '\0';
}

int read_line(FILE *fp, char* str, int limit) { //������ ��� ������ ���ڿ� �Է� �Լ�
	int ch, i = 0;
	char tmp = ' ';

	while ((ch = fgetc(fp)) != '\n' && ch != EOF) //getchar()�� ��ȯ���� int��!! ���
		if (i < limit - 1) {
			if (tmp != ' ' || ch != ' ') {
				str[i++] = tmp = ch;
			}
		}

	if (str[i - 1] == ' ')
		i--;

	str[i] = '\0';
	return i; //���ڿ� ���� ��ȯ
}
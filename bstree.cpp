#include "bstree.h"

void bstree::insert(room x, nodeptr& p)
{
	if (p == NULL)
	{
		p = new node;
		p->element = x;
		p->left = NULL;
		p->right = NULL;
		p->height = 0;
		if (p == NULL)
		{
			cout << "Out of Space\n" << endl;
		}
	}
	else
	{
		string xx = x.nom;
		string px = p->element.nom;
		if (xx < px)
		{
			insert(x, p->left);
			if ((bsheight(p->left) - bsheight(p->right)) == 2)
			{
				if (xx < px)
				{
					p = srl(p);
				}
				else
				{
					p = drl(p);
				}
			}
		}
		else if (xx > px)
		{
			insert(x, p->right);
			if ((bsheight(p->right) - bsheight(p->left)) == 2)
			{
				if (xx > px)
				{
					p = srr(p);
				}
				else
				{
					p = drr(p);
				}
			}
		}
		else
		{
			cout << "Элемет с таким номером уже существует\n" << endl;
		}
	}
	int m, n, d;
	m = bsheight(p->left);
	n = bsheight(p->right);
	d = maximum(m, n);
	p->height = d + 1;
}

room bstree::find(room x, nodeptr& p, bool *err)
{
	static room R;

	if (p == NULL)
	{
		cout << "Такой номер отсутствует!\n" << endl;
		*err = true;
	}
	else
	{
		string xx = x.nom;
		string px = p->element.nom;
		if (xx < px)
		{
			R = find(x, p->left, err);
		}
		else
		{
			if (xx > px)
			{
				R = find(x, p->right, err);
			}
			else
			{
				return p->element;
			}
		}
	}
	return R;
}

void bstree::makeempty(nodeptr& p)
{
	nodeptr d;
	if (p != NULL)
	{
		makeempty(p->left);
		makeempty(p->right);
		d = p;
		free(d);
		p = NULL;
	}
}

void bstree::del(room x, nodeptr& p)
{
	nodeptr d;
	if (p == NULL)
	{
		cout << "Простите, но такого номера нет\n" << endl;
	}
	else
	{
		string xx = x.nom;
		string px = p->element.nom;
		if (xx < px)
		{
			del(x, p->left);
		}
		else if (xx > px)
		{
			del(x, p->right);
		}
		else if ((p->left == NULL) && (p->right == NULL))
		{
			d = p;
			free(d);
			p = NULL;
			cout << "Элемент удален\n" << endl;
		}
		else if (p->left == NULL)
		{
			d = p;
			free(d);
			p = p->right;
			cout << "Элемент удален\n" << endl;
		}
		else if (p->right == NULL)
		{
			d = p;
			p = p->left;
			free(d);
			cout << "Элемент удален\n" << endl;
		}
		else
		{
			p->element = deletemin(p->right);
		}
	}
}

room bstree::deletemin(nodeptr& p)
{
	room c;
	if (p->left == NULL)
	{
		c = p->element;
		p = p->right;
		return c;
	}
	else
	{
		c = deletemin(p->left);
		return c;
	}
}

void bstree::preorder(nodeptr p)
{
	if (p != NULL)
	{
		cout << "\nНомер: " << p->element.nom << "\n";
		cout << "Колличество мест: " << p->element.guest_number << endl;
		cout << "Колличество комнат: " << p->element.room_number << endl;
		(p->element.toilet) ? cout << "Есть сануезел\n" : cout << "Санузел отсутствует\n";
		cout << "Обарудование: " << p->element.equipment << endl;
		preorder(p->left);
		preorder(p->right);
	}
}

void bstree::search_equip(nodeptr p, string need_equip,static bool*b)
{
	
	if (p != NULL)
	{
		char txt[256];
		char pat[256];
		for (int i = 0; i < p->element.equipment.length(); i++)
		{
			txt[i] = p->element.equipment[i];
			if (i == p->element.equipment.length() - 1)
				txt[i + 1] = '\0';
		}
		for (int i = 0; i < need_equip.length(); i++)
		{
			pat[i] = need_equip[i];
			if (i == need_equip.length() - 1)
				pat[i + 1] = '\0';
		}
		if (search(txt, pat))
		{
			if (!b)
				cout << "Найденные номера по вашему запросу:";
			*b = true;
			cout << "\nНомер: " << p->element.nom << "\n";
			cout << "Колличество мест: " << p->element.guest_number << endl;
			cout << "Колличество комнат: " << p->element.room_number << endl;
			(p->element.toilet) ? cout << "Есть сануезел\n" : cout << "Санузел отсутствует\n";
			cout << "Обарудование: " << p->element.equipment << endl;
		}
		search_equip(p->left, need_equip, b);
		search_equip(p->right, need_equip, b);
	}
}

int bstree::maximum(int value1, int value2)
{
	return ((value1 > value2) ? value1 : value2);
}

int bstree::bsheight(nodeptr p)
{
	int t;
	if (p == NULL)
	{
		return -1;
	}
	else
	{
		t = p->height;
		return t;
	}
}

nodeptr bstree::srl(nodeptr& p1)
{
	nodeptr p2;
	p2 = p1->left;
	p1->left = p2->right;
	p2->right = p1;
	p1->height = maximum(bsheight(p1->left), bsheight(p1->right)) + 1;
	p2->height = maximum(bsheight(p2->left), p1->height) + 1;
	return p2;
}

nodeptr bstree::srr(nodeptr& p1)
{
	nodeptr p2;
	p2 = p1->right;
	p1->right = p2->left;
	p2->left = p1;
	p1->height = maximum(bsheight(p1->left), bsheight(p1->right)) + 1;
	p2->height = maximum(p1->height, bsheight(p2->right)) + 1;
	return p2;
}

nodeptr bstree::drl(nodeptr& p1)
{
	p1->left = srr(p1->left);
	return srl(p1);
}

nodeptr bstree::drr(nodeptr& p1)
{
	p1->right = srl(p1->right);
	return srr(p1);
}

void bstree::badCharHeuristic(char* str, int size, int badchar[NO_OF_CHARS])
{
	int i;

	for (i = 0; i < NO_OF_CHARS; i++)
		badchar[i] = -1;

	for (i = 0; i < size; i++)
		badchar[-((int)str[i])] = i;
}

bool bstree::search(char* txt, char* pat)
 {

	int m = strlen(pat);
	int n = strlen(txt);

	int badchar[NO_OF_CHARS];

	badCharHeuristic(pat, m, badchar);

	int s = 0; 
	while (s <= (n - m))
	{
		int j = m - 1;

		while (j >= 0 && pat[j] == txt[s + j])
			j--;

		if (j < 0)
		{
			return true;
		}

		else
		{
			int index;
			if (txt[s + j] < 0)
				index = -txt[s + j];
			else
				index = txt[s + j];
			s += maximum(1, j - badchar[index]);
		}

	}
	return false;
}

bool operator>(room& first, nodeptr& second)
{
	string one, two;
	one = first.nom;
	two = second->element.nom;

	if (one > two) return true;
	else return false;
}

bool operator<(room& first, nodeptr& second)
{
	string one, two;
	one = first.nom;
	two = second->element.nom;

	if (one < two) return true;
	else return false;
}
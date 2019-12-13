#include "History.h"
#include <iostream>
#include "Arena.h"
using namespace std;

History::History(int nRows, int nCols)
{
	m_hRows = nRows;
	m_hCols = nCols;
	
	for (int i = 0; i < m_hRows; i++)		//set all grid spots to '.' for zero deaths
		for (int j = 0; j < m_hCols; j++)
			m_deaths[i][j] = '.';
}

bool History::record(int r, int c)
{
	if (r > m_hRows || r < 1 || c > m_hCols || c < 1)
		return false;

	if (m_deaths[r - 1][c - 1] == '.')
		m_deaths[r - 1][c - 1] = 'A';
	else if (m_deaths[r - 1][c - 1] == 'Z')
		m_deaths[r - 1][c - 1] = 'Z';
	else
		m_deaths[r - 1][c - 1]++;

	return true;
}

void History::display() const
{
	clearScreen();
	for (int i = 0; i < m_hRows; i++)
	{
		for (int j = 0; j < m_hCols; j++)
		{
			cout << m_deaths[i][j];
		}
		cout << endl;
	}
	cout << endl;
}
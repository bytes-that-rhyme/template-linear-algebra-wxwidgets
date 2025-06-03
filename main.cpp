// Start of wxWidgets "Hello World" Program
#include <wx/wx.h>

#include <armadillo>
#include "openblas/cblas.h"

using namespace arma;

//////// Tests

/**
Make sure Armadillo basic functionality works, by multiplying two random matrices.
*/
void TestArmadillo(std::ostream& log)
{
	log << "Testing Armadillo =====================" << std::endl;

	mat A(4, 5, fill::randu);
	mat B(4, 5, fill::randu);

	log << "Matrix A: " << A << std::endl;
	log << "Matrix B: " << B << std::endl;
	log << "A * transpose(B): " << A * B.t() << std::endl;
}

/**
Make sure Armadillo SuperLU is linked.
*/
void TestSuperLU(std::ostream& log)
{
	log << "Testing SuperLU =====================" << std::endl;

	// Define the size of the matrix and the right-hand side vector
	const int n = 5;

	// Create a sparse matrix A (n x n)
	arma::sp_mat A(n, n);

	// Assign values to the sparse matrix A
	A(0, 0) = 4.0; A(0, 1) = -1.0; A(0, 2) = 0.0; A(0, 3) = 0.0; A(0, 4) = 0.0;
	A(1, 0) = -1.0; A(1, 1) = 4.0; A(1, 2) = -1.0; A(1, 3) = 0.0; A(1, 4) = 0.0;
	A(2, 0) = 0.0; A(2, 1) = -1.0; A(2, 2) = 4.0; A(2, 3) = -1.0; A(2, 4) = 0.0;
	A(3, 0) = 0.0; A(3, 1) = 0.0; A(3, 2) = -1.0; A(3, 3) = 4.0; A(3, 4) = -1.0;
	A(4, 0) = 0.0; A(4, 1) = 0.0; A(4, 2) = 0.0; A(4, 3) = -1.0; A(4, 4) = 4.0;

	// Create a dense vector b (n x 1)
	arma::vec b(n);

	// Assign values to the vector b
	b(0) = 1.0;
	b(1) = 2.0;
	b(2) = 3.0;
	b(3) = 4.0;
	b(4) = 5.0;

	// Solve the sparse linear system Ax = b
	arma::vec x;

	// Use the SuperLU solver in Armadillo to solve the system
	bool success = arma::spsolve(x, A, b, "superlu");

	// Check if the solution was successful
	if (success) {
		log << "Solution x:" << std::endl;
		x.print(log);
	}
	else {
		log << "Failed to solve the sparse linear system." << std::endl;
	}
}

void TestLapack(std::ostream& log)
{
	log << "Testing Lapack =====================" << std::endl;

	// Define a positive-definite matrix A
	arma::mat A = { {4.0, 12.0, -16.0},
				   {12.0, 37.0, -43.0},
				   {-16.0, -43.0, 98.0} };

	// Matrix to hold the lower triangular matrix L
	arma::mat L;

	// Perform Cholesky decomposition
	if (arma::chol(L, A)) {
		log << "Matrix L:" << std::endl;
		L.print(log);
	}
	else {
		log << "Cholesky decomposition failed. The matrix might not be positive-definite." << std::endl;
	}
}

void TestArpack(std::ostream& log) {
	log << "Testing Arpack =====================" << std::endl;

	// Define the size of the matrix
	const int n = 100;

	// Create a sparse matrix A (n x n)
	arma::sp_mat A(n, n);

	// Fill the sparse matrix with some values
	for (int i = 0; i < n; ++i) {
		A(i, i) = 4.0;
		if (i > 0) {
			A(i, i - 1) = -1.0;
			A(i - 1, i) = -1.0;
		}
	}

	// Define the number of eigenvalues to compute
	const int nev = 5;

	// Vector to hold the computed eigenvalues
	arma::vec eigval;

	// Matrix to hold the computed eigenvectors
	arma::mat eigvec;

	// Use ARPACK through Armadillo to compute the eigenvalues and eigenvectors
	bool success = arma::eigs_sym(eigval, eigvec, A, nev);

	// Check if the computation was successful
	if (success) {
		log << "Eigenvalues:" << std::endl;
		eigval.print(log);

		log << "Eigenvectors:" << std::endl;
		eigvec.print(log);
	}
	else {
		log << "Failed to compute eigenvalues and eigenvectors." << std::endl;
	}
}

//////// wxWidgets GUI code


class MyApp : public wxApp
{
public:
	bool OnInit() override;
};

// Note: this includes the WinMain() function
wxIMPLEMENT_APP(MyApp);

class MyFrame : public wxFrame
{
public:
	MyFrame();

private:
	void OnHello(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);

	wxTextCtrl* m_textbox;
};

enum
{
	ID_Hello = 1,
	ID_Textbox = 2
};

bool MyApp::OnInit()
{
	MyFrame* frame = new MyFrame();
	frame->Show(true);
	return true;
}

MyFrame::MyFrame()
	: wxFrame(nullptr, wxID_ANY, "Hello World")
{
	wxMenu* menuFile = new wxMenu;
	menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
		"Help string shown in status bar for this menu item");
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);

	wxMenu* menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT);

	wxMenuBar* menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuHelp, "&Help");

	SetMenuBar(menuBar);

	CreateStatusBar();
	SetStatusText("Welcome to wxWidgets!");

	m_textbox = new wxTextCtrl(this, ID_Textbox, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE,
		wxDefaultValidator, wxTextCtrlNameStr);

	std::ostringstream log;

	TestArmadillo(log);
	TestSuperLU(log);
	TestLapack(log);
	TestArpack(log);

	m_textbox->WriteText(log.str());

	Bind(wxEVT_MENU, &MyFrame::OnHello, this, ID_Hello);
	Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
	Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
}


void MyFrame::OnExit(wxCommandEvent& event)
{
	Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
	wxMessageBox("This is a wxWidgets Hello World example",
		"About Hello World", wxOK | wxICON_INFORMATION);
}

void MyFrame::OnHello(wxCommandEvent& event)
{
	wxLogMessage("Hello world from wxWidgets!");
}
class AFX_CLASS_EXPORT CStringDlg : public CDialog
{
// Construction
public:
        CStringDlg(CWnd* pParent = NULL); // standard constructor

// Dialog Data
        //{{AFX_DATA(CStringDlg)
        enum { IDD = IDD_STRINGDIALOG };
        CString m_String;
        //}}AFX_DATA

        void SetStringBeschreibung(CString S);

// Implementation
protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

        // Generated message map functions
        //{{AFX_MSG(CStringDlg)
                // NOTE: the ClassWizard will add member functions here
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};


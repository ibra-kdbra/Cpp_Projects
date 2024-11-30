#ifndef GUI_APP_H
#define GUI_APP_H

#include <QMainWindow>
#include <QLineEdit>
#include <QTextEdit>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPushButton>
#include <QSplitter>
#include <QVBoxLayout>
#include "TreeNode.h" 

class GuiApp : public QMainWindow {
    Q_OBJECT

public:
    explicit GuiApp(QWidget *parent = nullptr);
    ~GuiApp();

private slots:
    void loadTreeFromFile();
    void inputTreeManually();
    void drawTree();
    void applyAlgorithm(int choice);

private:
    TreeNode *root;
    QGraphicsScene *scene;

    // UI Elements
    QGraphicsView *graphicsView;
    QTextEdit *outputArea;
    QLineEdit *inputField;
    QPushButton *manualInputButton;
    QPushButton *loadFileButton;
    QPushButton *drawButton;
    QPushButton *algorithmButtons[5];

    TreeNode* buildTreeFromInput(const QString &input);
    void clearUI();
};

#endif 

#include "GuiApp.h"
#include <QMessageBox>
#include <QGraphicsTextItem>
#include <QInputDialog>
#include <QSplitter>
#include <QGuiApplication>
#include <QScreen>
#include <QFileDialog>
#include <QFile>
#include <QIODevice>
#include <sstream>
#include "Algorithms.h"

GuiApp::GuiApp(QWidget *parent)
    : QMainWindow(parent), root(nullptr), scene(new QGraphicsScene(this)) {
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal, this);

    QVBoxLayout *leftLayout = new QVBoxLayout();
    QWidget *leftWidget = new QWidget();
    leftWidget->setLayout(leftLayout);

    graphicsView = new QGraphicsView(this);
    graphicsView->setMinimumSize(400, 300);

    outputArea = new QTextEdit(this);
    outputArea->setReadOnly(true);
    outputArea->setFixedHeight(150);

    leftLayout->addWidget(graphicsView);
    leftLayout->addWidget(outputArea);

    QVBoxLayout *rightLayout = new QVBoxLayout();
    QWidget *rightWidget = new QWidget();
    rightWidget->setLayout(rightLayout);

    inputField = new QLineEdit(this);
    inputField->setPlaceholderText("Enter tree data manually...");
    inputField->setFixedWidth(200);

    manualInputButton = new QPushButton("Manually Input Tree", this);
    manualInputButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    manualInputButton->adjustSize();

    loadFileButton = new QPushButton("Load Tree from File", this);
    loadFileButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    loadFileButton->adjustSize();

    drawButton = new QPushButton("Draw Tree", this);
    drawButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    drawButton->adjustSize();

    QStringList algorithmNames = {
        "Diameter of Binary Tree",
        "Equal Tree Partition",
        "Flatten Binary Tree",
        "Invert Binary Tree",
        "Lowest Common Ancestor (LCA)"
    };

    for (int i = 0; i < 5; ++i) {
        algorithmButtons[i] = new QPushButton(algorithmNames[i], this);
        algorithmButtons[i]->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        algorithmButtons[i]->adjustSize();
        connect(algorithmButtons[i], &QPushButton::clicked, [=]() { applyAlgorithm(i + 1); });

        rightLayout->addWidget(algorithmButtons[i]);
    }

    rightLayout->addWidget(manualInputButton, 0, Qt::AlignLeft);
    rightLayout->addWidget(loadFileButton, 0, Qt::AlignLeft);
    rightLayout->addWidget(drawButton, 0, Qt::AlignLeft);

    rightLayout->addWidget(inputField, 0, Qt::AlignTop | Qt::AlignHCenter);

    mainSplitter->addWidget(leftWidget);
    mainSplitter->addWidget(rightWidget);

    mainLayout->addWidget(mainSplitter);

    setCentralWidget(centralWidget);

    setWindowTitle("Binary Tree Visualizer");
    resize(800, 600);

    connect(loadFileButton, &QPushButton::clicked, this, &GuiApp::loadTreeFromFile);
    connect(drawButton, &QPushButton::clicked, this, &GuiApp::drawTree);
    connect(manualInputButton, &QPushButton::clicked, this, &GuiApp::inputTreeManually);
}

GuiApp::~GuiApp() {
    delete root;
}

void GuiApp::loadTreeFromFile() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open Tree File", "", "Text Files (*.txt *.md)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Could not open file.");
        return;
    }

    QString content = file.readAll();
    file.close();

    root = buildTreeFromInput(content);
    if (root) {
        outputArea->setText("Tree successfully loaded.");
    } else {
        QMessageBox::warning(this, "Error", "Failed to build tree from file.");
    }
}

void GuiApp::inputTreeManually() {
    QString input = inputField->text().trimmed();
    if (input.isEmpty()) {
        QMessageBox::warning(this, "Error", "Input field is empty!");
        return;
    }

    root = buildTreeFromInput(input);
    if (root) {
        outputArea->setText("Tree successfully created.");
        drawTree();
    } else {
        QMessageBox::warning(this, "Error", "Failed to build tree from input.");
    }
}

void GuiApp::drawTree() {
    if (!root) {
        QMessageBox::warning(this, "Error", "Tree is empty!");
        return;
    }

    scene->clear();

    std::function<void(TreeNode*, int, int, int)> drawNode = [&](TreeNode* node, int x, int y, int offset) {
        if (!node) return;

        int radius = 20;
        scene->addEllipse(x - radius, y - radius, 2 * radius, 2 * radius);
        QGraphicsTextItem* textItem = scene->addText(QString::number(node->val));
        textItem->setPos(x - radius / 2, y - radius / 2);

        if (node->left) {
            scene->addLine(x, y + radius, x - offset, y + 70 - radius);
            drawNode(node->left, x - offset, y + 70, offset / 2);
        }

        if (node->right) {
            scene->addLine(x, y + radius, x + offset, y + 70 - radius);
            drawNode(node->right, x + offset, y + 70, offset / 2);
        }
    };

    drawNode(root, graphicsView->width() / 2, 50, 100);
    graphicsView->setScene(scene);
}


TreeNode* GuiApp::buildTreeFromInput(const QString& input) {
    if (input.isEmpty()) return nullptr;

    QStringList nodes = input.split(',', Qt::SkipEmptyParts);
    if (nodes.isEmpty()) return nullptr;

    TreeNode* root = new TreeNode(nodes[0].toInt());
    QList<TreeNode*> queue;
    queue.append(root);

    int i = 1;
    while (!queue.isEmpty() && i < nodes.size()) {
        TreeNode* current = queue.takeFirst();
        if (nodes[i] != "null") {
            current->left = new TreeNode(nodes[i].toInt());
            queue.append(current->left);
        }
        ++i;
        if (i < nodes.size() && nodes[i] != "null") {
            current->right = new TreeNode(nodes[i].toInt());
            queue.append(current->right);
        }
        ++i;
    }
    return root;
}

void GuiApp::applyAlgorithm(int choice) {
    if (!root) {
        QMessageBox::warning(this, "Error", "Tree is empty!");
        return;
    }

    QString result;
    switch (choice) {
        case 1:
            result = QString("Diameter of the tree: %1").arg(solveDiameter(root));
            break;
        case 2:
            result = isPartitionPossible(root) ? "Equal partition possible" : "Equal partition NOT possible";
            break;
        case 3:
            root = flattenBinaryTree(root);
            result = "Binary tree has been flattened.";
            break;
        case 4:
            root = invertBinaryTree(root);
            result = "Binary tree has been inverted.";
            break;
        case 5:
            {
                bool ok1, ok2;
                int n1 = QInputDialog::getInt(this, "Input", "Enter first node for LCA:", 0, 0, 10000, 1, &ok1);
                int n2 = QInputDialog::getInt(this, "Input", "Enter second node for LCA:", 0, 0, 10000, 1, &ok2);

                if (ok1 && ok2) {
                    int lca_result_first = findLCA(root, n1, n2);
                    if (lca_result_first != -1) {
                        result = QString("LCA (First Approach): %1").arg(lca_result_first);
                    } else {
                        result = "No common ancestor found for the provided nodes (First Approach).";
                    }

                    int lca_result_second = lca(root, n1, n2);
                    if (lca_result_second != -1) {
                        result += "\nLCA (Second Approach): " + QString::number(lca_result_second);
                    } else {
                        result += "\nNo common ancestor found for the provided nodes (Second Approach).";
                    }
                } else {
                    result = "Invalid node input.";
                }
            }
            break;

        default:
            result = "Invalid choice.";
            break;
    }

    outputArea->setText(result);
    drawTree();
}

void GuiApp::clearUI() {
    scene->clear();
    outputArea->clear();
}


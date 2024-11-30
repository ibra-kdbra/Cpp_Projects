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
#include "TreeNode.h"

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
        "Lowest Common Ancestor (LCA)",
        "Single Traversal",
        "Stack-Based Traversal",
        "Distance Between Nodes",
        "Sorted Array to BST"
    };

    for (int i = 0; i < algorithmNames.size(); ++i) {
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
                int n1, n2;
                if (getLCAInput(n1, n2)) {
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

        case 6:
            {
                int B, C;
                if (getSingleTraversalInput(B, C)) {
                    int result_single_traversal = solverSingleTraversal(root, B, C);
                    result = QString("Single Traversal Result: %1").arg(result_single_traversal);
                } else {
                    result = "Invalid input for B or C.";
                }
            }
            break;

        case 7:
            {
                TreeNode* secRoot = getSecondTreeInput();
                if (secRoot) {
                    int result_stack_traversal = solverWithStacks(root, secRoot);
                    result = QString("Stack-based Traversal Result: %1").arg(result_stack_traversal);
                } else {
                    result = "Invalid second tree root input.";
                }
            }
            break;

        case 8:
            {
                int node1, node2;
                if (getDistanceInput(node1, node2)) {
                    int distance = distanceBetweenTwoNodes(root, node1, node2);
                    result = QString("Distance between nodes: %1").arg(distance);
                } else {
                    result = "Invalid node input.";
                }
            }
            break;

        case 9:
            {
                std::vector<int> sortedArray;
                if (getSortedArrayInput(sortedArray)) {
                    TreeNode* bstRoot = sortedArrayToBST(sortedArray);
                    result = "Sorted array has been converted to a balanced BST.";
                    root = bstRoot;  // Optionally update the root if needed
                } else {
                    result = "Invalid sorted array input.";
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

// Helper Functions for User Inputs

bool GuiApp::getLCAInput(int &n1, int &n2) {
    bool ok1, ok2;
    n1 = QInputDialog::getInt(this, "Input", "Enter first node for LCA:", 0, 0, 10000, 1, &ok1);
    n2 = QInputDialog::getInt(this, "Input", "Enter second node for LCA:", 0, 0, 10000, 1, &ok2);
    return ok1 && ok2;
}

bool GuiApp::getSingleTraversalInput(int &B, int &C) {
    bool ok1, ok2;
    B = QInputDialog::getInt(this, "Input", "Enter the first value (B):", 0, 0, 10000, 1, &ok1);
    C = QInputDialog::getInt(this, "Input", "Enter the second value (C):", 0, 0, 10000, 1, &ok2);
    return ok1 && ok2;
}

TreeNode* GuiApp::getSecondTreeInput() {
    bool ok;
    int secRootVal = QInputDialog::getInt(this, "Input", "Enter second tree root value:", 0, 0, 10000, 1, &ok);
    if (ok) {
        return findNode(root, secRootVal);  // Assuming `findNode` is a function that searches for a node by value.
    }
    return nullptr;
}


bool GuiApp::getDistanceInput(int &node1, int &node2) {
    bool ok1, ok2;
    node1 = QInputDialog::getInt(this, "Input", "Enter first node:", 0, 0, 10000, 1, &ok1);
    node2 = QInputDialog::getInt(this, "Input", "Enter second node:", 0, 0, 10000, 1, &ok2);
    return ok1 && ok2;
}

bool GuiApp::getSortedArrayInput(std::vector<int> &sortedArray) {
    QString arrayInput = QInputDialog::getText(this, "Input", "Enter sorted array (comma separated):");
    QStringList arrStrings = arrayInput.split(",", Qt::SkipEmptyParts);
    for (const auto& val : arrStrings) {
        sortedArray.push_back(val.toInt());
    }
    return !sortedArray.empty();
}


void GuiApp::clearUI() {
    scene->clear();
    outputArea->clear();
}


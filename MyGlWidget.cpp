#include "MyGlWidget.h"
#include "MainWindow.h"


#include <QtGui/QMouseEvent>
//#include "My\GLWidget.h"
#include <QDebug>
#include <Matrix.h>
#include <Vector.h>

MyGlWidget::MyGlWidget(QWidget *parent)
  : QGLWidget(parent), selectMode(0)
{
    setMouseTracking(true);

  timer.setInterval(200);
  timer.setSingleShot(true);
  connect(&timer, SIGNAL(timeout()), this, SLOT(mouseSingleClickEvent()));
  doubleClicked = 0;    
}

MyGlWidget::~MyGlWidget()
{
}

void MyGlWidget::setModel(MainWindow *theM)
{
    theModel = theM;
}


void
MyGlWidget::drawNode(int tag, float x1, float y1, int numPixels, float r, float g, float b)
{

    if (selectMode == 0) {
        glPointSize(numPixels);
        glColor3f(r, g, b);
        glBegin(GL_POINTS);
        glVertex2f(x1, y1);
        glEnd();
    } else {
        if (tag != 0) {
            glPointSize(numPixels);
            int r1 = (tag & 0x000000FF) >>  0;
            int g1 = (tag & 0x0000FF00) >>  8;
            int b1 = (tag & 0x00FF0000) >> 16;
                   glColor3f(r1/255.0,g1/255.0,b1/255.0);
    glPointSize(numPixels);
        glBegin(GL_POINTS);
        glVertex3f(x1, y1, 0.0);
        glEnd();
        }
    }
}

void
MyGlWidget::drawLine(int tag, float x1, float y1, float x2, float y2, float thick, float r, float g, float b)
{
    if (selectMode == 0) {
        glLineWidth(thick);
        glColor3f(r, g, b);
        glBegin(GL_LINES);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
        glEnd();
    } else {
        if (tag != 0) {
            int r1 = (tag & 0x000000FF) >>  0;
            int g1 = (tag & 0x0000FF00) >>  8;
            int b1 = (tag & 0x00FF0000) >> 16;
                   glColor3f(r1/255.0,g1/255.0,b1/255.0);
        glLineWidth(thick);
        glBegin(GL_LINES);
        glVertex3f(x1, y1, 0.0);
        glVertex3f(x2, y2, 0.0);
        glEnd();
        }
    }
}


void MyGlWidget::initializeGL() {
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_COLOR_MATERIAL);
    glEnable(GL_BLEND);
    glEnable(GL_POLYGON_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(1, 1, 1, 0);
}

void MyGlWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (theModel != 0) {
      float heightB = theModel->getHeight();
      float maxDisp = theModel->getMaxDisp();
      float bounH = heightB/20;
      if (maxDisp == 0)
	maxDisp = 10.0;
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
#ifdef QT_OPENGL_ES_1
      glOrthof(-maxDisp, +maxDisp, -bounH, bounH+heightB, -15, 15.0);
#else
      glOrtho(-maxDisp, +maxDisp, -bounH, heightB+bounH, -15, 15.0);
#endif
      //qDebug() << maxDisp << " " << bounH << " " << heightB;
    } else
      glOrtho(0, 6, 0, 6, -15, 15); // set origin to bottom left corner

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void MyGlWidget::update() {

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (theModel != 0) {
      float heightB = theModel->getHeight();
      float maxDisp = theModel->getMaxDisp();
      float bounH = heightB/20;
      if (maxDisp == 0)
        maxDisp = 10.0;
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
#ifdef QT_OPENGL_ES_1
      glOrthof(-maxDisp, +maxDisp, -bounH, bounH+heightB, -15, 15.0);
#else
      glOrtho(-maxDisp, +maxDisp, -bounH, heightB+bounH, -15, 15.0);
#endif
      //qDebug() << maxDisp << " " << bounH << " " << heightB;
    } else
      glOrtho(0, 6, 0, 6, -15, 15); // set origin to bottom left corner

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    this->QGLWidget::update();
}

void MyGlWidget::paintGL() {

    glClear(GL_COLOR_BUFFER_BIT);
    
    if (theModel != 0)
      return theModel->draw(this);

    glColor3f(1,0,0);
    glBegin(GL_POLYGON);
    glVertex2f(0,0);
    glVertex2f(0, 2);
    glVertex2f(2,0);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(2,2);
    glVertex2f(2, 4);
    glVertex2f(4,2);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(4,4);
    glVertex2f(4, 6);
    glVertex2f(6,4);
    glEnd();

    this->drawLine(10, 0, 2, 2, 2, 2, 0, 1, 0);
    
    /*
    glLineWidth(5);
     glColor3f(0,0,1);
    glBegin(GL_LINES);
    glVertex2f(0,2);
    glVertex2f(2,2);
    glEnd();
*/
     this->drawLine(10, 2, 4, 4, 4, 2, 0, 0, 1);
    glBegin(GL_LINES);
    glVertex2f(2,4);
    glVertex2f(4,4);
    glEnd();

    this->drawNode(10, 2, 4, 10,1,0,0);

}

void MyGlWidget::keyPressEvent(QKeyEvent* event) {
    switch(event->key()) {
    case Qt::Key_Escape:
        close();
        break;
    default:
        event->ignore();
        break;
    }
}

void MyGlWidget::mousePressEvent(QMouseEvent *event)
{
    // Save mouse press position
    mousePressPosition = event->localPos();
    
    if(event->buttons() & Qt::LeftButton) {
      clickedLeft = true;
    } else {
      clickedLeft = false;
    }
  if(doubleClicked){
    doubleClicked = 1;
  } else{
    timer.start();
  }
  QWidget::mousePressEvent(event);
}

void MyGlWidget::mouseReleaseEvent(QMouseEvent *event)
{
    mouseReleasePosition = event->localPos();

    //
    // need to determine world coords represented by mouse position in view world
    //
    GLint viewport[4]; //var to hold the viewport info
    GLdouble modelview[16]; //var to hold the modelview info
    GLdouble projection[16]; //var to hold the projection matrix info

    glGetDoublev( GL_MODELVIEW_MATRIX, modelview ); //get the modelview info
    glGetDoublev( GL_PROJECTION_MATRIX, projection ); //get the projection matrix info
    glGetIntegerv( GL_VIEWPORT, viewport ); //get the viewport info

    for (int i=0; i<4; i++)
        viewport[i] /= devicePixelRatio();

    // get the world coordinates from the screen coordinates, in QT5.8 we have the unproject function
    // Qt5.8: QVector3D world = QVector3D(mousePressPosition.x(), mousePressPosition.y(), 0).unproject(modelview, projection, viewport, &x, &y, &z);

    Matrix viewPortMatrix(4,4);
    Matrix modelViewMatrix(modelview,4,4);
    Matrix projectionMatrix(projection, 4,4);
    Matrix A = projectionMatrix*modelViewMatrix;

    Vector in(4);
    float xPos = mousePressPosition.x();
    float yPos = viewport[3] - mousePressPosition.y() -1;

    in[0]=2*(xPos-viewport[0])/(1.0*viewport[2]) - 1.0;
    in[1]=2*(yPos-viewport[1])/(1.0*viewport[3]) - 1.0;
    in[2]=0.0;
    in[3]=1.0;
    Vector pressCrd(4);
    A.Solve(in,pressCrd);

    xPos = mouseReleasePosition.x();
    yPos = viewport[3] - mouseReleasePosition.y() -1;

    in[0]=2*(xPos-viewport[0])/(1.0*viewport[2]) - 1.0;
    in[1]=2*(yPos-viewport[1])/(1.0*viewport[3]) - 1.0;
    in[2]=0.0;
    in[3]=1.0;
    Vector releaseCrd(4);
    A.Solve(in,releaseCrd);

    opserr << releaseCrd;

    // given press and release coordinated, set vectors containing floors and stories that can be edited
    //theModel->setSelectionBoundary()

}

void MyGlWidget::mouseMoveEvent(QMouseEvent *event)
{
    /*
  int dx = event->x() - lastPos.x();
  int dy = event->y() - lastPos.y();

  if (event->buttons() & Qt::LeftButton) {
    //    setXRotation(xRot + 8 * dy);
    //    setYRotation(yRot + 8 * dx);
  } else if (event->buttons() & Qt::RightButton) {
    //    setXRotation(xRot + 8 * dy);
    //    setZRotation(zRot + 8 * dx);
  }

  lastPos = event->pos();
  */
}

void MyGlWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
  qDebug() << clickedLeft << " DOUBE CLICK" << doubleClicked;
  
  timer.stop();
  doubleClicked = 0; // this is to discard another press event coming

}

void MyGlWidget::mouseSingleClickEvent(void) {
  qDebug() << clickedLeft << " SINGLE CLICK ";
}


#pragma once
#include <QObject>
#include <QVector>

class SensorSettingsModel : public QObject {
    Q_OBJECT

    Q_PROPERTY(double maxRange READ maxRange WRITE setMaxRange NOTIFY modelChanged)
    Q_PROPERTY(double lambda READ lambda WRITE setLambda NOTIFY modelChanged)
    Q_PROPERTY(double sigma READ sigma WRITE setSigma NOTIFY modelChanged)
    Q_PROPERTY(double a0 READ a0 WRITE setA0 NOTIFY modelChanged)
    Q_PROPERTY(double a1 READ a1 WRITE setA1 NOTIFY modelChanged)
    Q_PROPERTY(double a2 READ a2 WRITE setA2 NOTIFY modelChanged)
    Q_PROPERTY(double a3 READ a3 WRITE setA3 NOTIFY modelChanged)

    Q_PROPERTY(QList<double> samples READ samples)

signals:
    void modelChanged();

  public:
    double maxRange() const { return m_maxRange; }
    double lambda() const { return m_lambda; }
    double sigma() const { return m_sigma; }
    double a0() const { return m_a0; }
    double a1() const { return m_a1; }
    double a2() const { return m_a2; }
    double a3() const { return m_a3; }

    void setMaxRange(double v) {
        if (m_maxRange == v)
            return;
        m_maxRange = v;
        recalculate();
    }
    void setLambda(double v) {
        if (m_lambda == v)
            return;
        m_lambda = v;
        recalculate();
    }
    void setSigma(double v) {
        if (m_sigma == v)
            return;
        m_sigma = v;
        recalculate();
    }
    void setA0(double v) {
        if (m_a0 == v)
            return;
        m_a0 = v;
        recalculate();
    }
    void setA1(double v) {
        if (m_a1 == v)
            return;
        m_a1 = v;
        recalculate();
    }
    void setA2(double v) {
        if (m_a2 == v)
            return;
        m_a2 = v;
        recalculate();
    }
    void setA3(double v) {
        if (m_a3 == v)
            return;
        m_a3 = v;
        recalculate();
    }

    const QList<double> &samples() const;

  private:
    void recalculate();

  private:
    double m_maxRange;
    double m_lambda;
    double m_sigma;
    double m_a0;
    double m_a1;
    double m_a2;
    double m_a3;

    QList<double> m_samples;
};
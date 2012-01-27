#ifndef FREQUENCYANALYZER_P_H
#define FREQUENCYANALYZER_P_H
#include "frequencyanalyzer.h"
#include "measureFreqXcorr.h"

#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <QAudioFormat>

#include <QDebug>
#include <QDataStream>

static const int frameSize = 8192;
static const int sampleSize = 8192/sizeof(int32_T)*sizeof(int);

class FrequencyAnalyzer;
class FrequencyAnalyzerPrivate
{
	Q_DECLARE_PUBLIC(FrequencyAnalyzer)
public:
	FrequencyAnalyzerPrivate(FrequencyAnalyzer *q) : q_ptr(q), input(0), device(0), sampling(-1) {}
	~FrequencyAnalyzerPrivate() {}

	FrequencyAnalyzer *q_ptr;
	QAudioInput *input;
	QIODevice *device;
	QByteArray buffer;
	real32_T frequency;
	int32_T sampling;

	void _q_onReadyRead()
	{
		buffer.append(device->readAll());
		if (buffer.count() >= sampleSize) {
			//QByteArray ref = buffer.left(); //black magic((
			//int32_T *s = reinterpret_cast<int32_T*>(ref.data());

			//TODO optimize me
			int32_T s[frameSize];
			QDataStream stream(buffer);
			for (int i = 0; i!= frameSize; i++)
				stream >> s[i];

			real32_T snr;
			creal32_T w[frameSize];
			real32_T now;
			measureFreqXcorr(s, sampling, &now, &snr, w);
			buffer.clear();

			if (qAbs(now - frequency) > 10) {
				qDebug() << "Received full fft frame with size " << buffer.count() << ". Processing...";
				emit q_func()->currentFrequencyChanged(frequency);
			}
		}
	}
};

#endif // FREQUENCYANALYZER_P_H

//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Mon Aug 17 02:39:28 PDT 2015
// Last Modified: Mon Aug 17 02:39:32 PDT 2015
// Filename:      HumdrumFileStructure.cpp
// URL:           https://github.com/craigsapp/humlib/blob/master/src/HumdrumFileStructure.cpp
// Syntax:        C++11; humlib
// vim:           syntax=cpp ts=3 noexpandtab nowrap
//
// Description:   Used to further process HumdrumFileBase content, primarily
//                rhythmic analyses, but also parses global and local
//                token parameters.  The HumdrumFileContent class does
//                further analysis of the Humdrum data, primary of specific
//                data content rather than general structural analysis.
//

#include "HumdrumFileStructure.h"
#include "Convert.h"

#include <string.h>

#include <algorithm>
#include <sstream>

using namespace std;

namespace hum {

// START_MERGE

//////////////////////////////
//
// HumdrumFileStructure::HumdrumFileStructure -- HumdrumFileStructure
//     constructor.
//

HumdrumFileStructure::HumdrumFileStructure(void) {
	// do nothing
}

HumdrumFileStructure::HumdrumFileStructure(const string& filename) :
		HumdrumFileBase() {
	read(filename);
}

HumdrumFileStructure::HumdrumFileStructure(istream& contents) :
		HumdrumFileBase() {
	read(contents);
}



//////////////////////////////
//
// HumdrumFileStructure::~HumdrumFileStructure -- HumdrumFileStructure
//     deconstructor.
//

HumdrumFileStructure::~HumdrumFileStructure() {
	// do nothing
}



//////////////////////////////
//
// HumdrumFileStructure::read --  Read the contents of a file from a file or
//   istream.  The file's structure is analyzed, and then the rhythmic structure
//   is calculated.
//


bool HumdrumFileStructure::read(istream& contents) {
	m_displayError = false;
	if (!readNoRhythm(contents)) {
		return isValid();
	}
	return analyzeStructure();
}


bool HumdrumFileStructure::read(const char* filename) {
	m_displayError = false;
	if (!readNoRhythm(filename)) {
		return isValid();
	}
	return analyzeStructure();
}


bool HumdrumFileStructure::read(const string& filename) {
	m_displayError = false;
	if (!readNoRhythm(filename)) {
		return isValid();
	}
	return analyzeStructure();
}



//////////////////////////////
//
// HumdrumFileStructure::readCsv --  Read the contents of a file from a file or
//   istream in CSV format.  The file's structure is analyzed, and then the
//   rhythmic structure is calculated.
// default value: separator = ","
//


bool HumdrumFileStructure::readCsv(istream& contents,
		const string& separator) {
	m_displayError = false;
	if (!readNoRhythmCsv(contents, separator)) {
		return isValid();
	}
	return analyzeStructure();
}


bool HumdrumFileStructure::readCsv(const char* filename,
		const string& separator) {
	m_displayError = false;
	if (!readNoRhythmCsv(filename, separator)) {
		return isValid();
	}
	return analyzeStructure();
}


bool HumdrumFileStructure::readCsv(const string& filename,
		const string& separator) {
	m_displayError = false;
	if (!readNoRhythmCsv(filename, separator)) {
		return isValid();
	}
	return analyzeStructure();
}



//////////////////////////////
//
// HumdrumFileStructure::readString -- Read the contents from a string.
//    Similar to HumdrumFileStructure::read, but for string data.
//

bool HumdrumFileStructure::readString(const char* contents) {
	m_displayError = false;
	if (!HumdrumFileBase::readString(contents)) {
		return isValid();
	}
	return analyzeStructure();
}


bool HumdrumFileStructure::readString(const string& contents) {
	m_displayError = false;
	if (!HumdrumFileBase::readString(contents)) {
		return isValid();
	}
	return analyzeStructure();
}



//////////////////////////////
//
// HumdrumFileStructure::readStringCsv -- Read the contents from a string.
//    Similar to HumdrumFileStructure::read, but for string data.
// default value: separator = ","
//

bool HumdrumFileStructure::readStringCsv(const char* contents,
		const string& separator) {
	m_displayError = false;
	if (!HumdrumFileBase::readStringCsv(contents, separator)) {
		return isValid();
	}
	return analyzeStructure();
}


bool HumdrumFileStructure::readStringCsv(const string& contents,
		const string& separator) {
	m_displayError = false;
	if (!HumdrumFileBase::readStringCsv(contents, separator)) {
		return isValid();
	}
	return analyzeStructure();
}



//////////////////////////////
//
// HumdrumFileStructure::analyzeStructure -- Analyze global/local
//    parameters and rhythmic structure.
//

bool HumdrumFileStructure::analyzeStructure(void) {
	m_structure_analyzed = false;
	if (!m_strands_analyzed) {
		if (!analyzeStrands()          ) { return isValid(); }
	}
	if (!analyzeGlobalParameters() ) { return isValid(); }
	if (!analyzeLocalParameters()  ) { return isValid(); }
	if (!analyzeTokenDurations()   ) { return isValid(); }
	if (!analyzeTokenDurations()   ) { return isValid(); }
	m_structure_analyzed = true;
	if (!analyzeRhythmStructure()  ) { return isValid(); }
	analyzeSignifiers();
	return isValid();
}



//////////////////////////////
//
// HumdrumFileStructure::analyzeStructureNoRhythm -- Analyze global/local
//    parameters but not rhythmic structure.
//

bool HumdrumFileStructure::analyzeStructureNoRhythm(void) {
	m_structure_analyzed = true;
	if (!m_strands_analyzed) {
		if (!analyzeStrands()          ) { return isValid(); }
	}
	if (!analyzeGlobalParameters() ) { return isValid(); }
	if (!analyzeLocalParameters()  ) { return isValid(); }
	if (!analyzeTokenDurations()   ) { return isValid(); }
	analyzeSignifiers();
	return isValid();
}



/////////////////////////////
//
// HumdrumFileStructure::analyzeRhythmStructure --
//

bool HumdrumFileStructure::analyzeRhythmStructure(void) {
	m_rhythm_analyzed = true;
	setLineRhythmAnalyzed();
	if (!isStructureAnalyzed()) {
		if (!analyzeStructureNoRhythm()) { return isValid(); }
	}

	HTp firstspine = getSpineStart(0);
	if (firstspine && firstspine->isDataType("**recip")) {
		assignRhythmFromRecip(firstspine);
	} else {
		if (!analyzeRhythm()           ) { return isValid(); }
		if (!analyzeDurationsOfNonRhythmicSpines()) { return isValid(); }
	}
	return isValid();
}



//////////////////////////////
//
// HumdrumFileStructure::assignRhythmFromRecip --
//

bool HumdrumFileStructure::assignRhythmFromRecip(HTp spinestart) {
	HTp current = spinestart;

	HumNum duration;
	while (current) {
		if (!current->isData()) {
			current = current->getNextToken();
			continue;
		}
		if (current->isNull()) {
			// This should not occur in a well-formed **recip spine, but
			// treat as a zero duration.
			current = current->getNextToken();
			continue;
		}

		if (strchr(current->c_str(), 'q') != NULL) {
			duration = 0;
		} else {
			duration = Convert::recipToDuration((string)*current);
		}
		current->getLine()->setDuration(duration);
		current = current->getNextToken();
	}

	// now go back and set the absolute position from the start of
	// the file.
	HumNum sum = 0;
	HumdrumFileStructure& hfile = *this;
	for (int i=0; i<getLineCount(); i++) {
		hfile[i].setDurationFromStart(sum);
		if (hfile[i].getDuration() < 0) {
			hfile[i].setDuration(0);
		}
		sum += hfile[i].getDuration();
	}

	// Analyze durations to/from barlines:
	if (!analyzeMeter()) { return false; }
	if (!analyzeNonNullDataTokens()) { return false; }
	return true;
}



//////////////////////////////
//
// HumdrumFileStructure::readNoRhythm -- Similar to the read() functions, but
//    does not parse rhythm (or parameters).
//

bool HumdrumFileStructure::readNoRhythm(istream& infile) {
	return HumdrumFileBase::read(infile);
}


bool HumdrumFileStructure::readNoRhythm(const char* filename) {
	return HumdrumFileBase::read(filename);
}


bool HumdrumFileStructure::readNoRhythm(const string& filename) {
	return HumdrumFileBase::read(filename);
}



//////////////////////////////
//
// HumdrumFileStructure::readNoRhythmCsv -- Similar to the readCsv()
//    functions, but does not parse rhythm (or parameters).
// default value: separator = ","
//

bool HumdrumFileStructure::readNoRhythmCsv(istream& infile,
		const string& seperator) {
	return HumdrumFileBase::readCsv(infile);
}


bool HumdrumFileStructure::readNoRhythmCsv(const char* filename,
		const string& seperator) {
	return HumdrumFileBase::readCsv(filename);
}


bool HumdrumFileStructure::readNoRhythmCsv(const string& filename,
		const string& seperator) {
	return HumdrumFileBase::readCsv(filename);
}



//////////////////////////////
//
// HumdrumFileStructure::readStringNoRhythm -- Read a string, but
//   do not analyze the rhythm (or parameters) in the read data.
//


bool HumdrumFileStructure::readStringNoRhythm(const char* contents) {
	return HumdrumFileBase::readString(contents);
}


bool HumdrumFileStructure::readStringNoRhythm(const string& contents) {
	return HumdrumFileBase::readString(contents);
}



//////////////////////////////
//
// HumdrumFileStructure::readStringNoRhythmCsv -- Read a string, but
//   do not analyze the rhythm (or parameters) in the read data.
// default value: separator = ","
//


bool HumdrumFileStructure::readStringNoRhythmCsv(const char* contents,
		const string& separator) {
	return HumdrumFileBase::readStringCsv(contents);
}


bool HumdrumFileStructure::readStringNoRhythmCsv(const string& contents,
		const string& separator) {
	return HumdrumFileBase::readStringCsv(contents);
}



//////////////////////////////
//
// HumdrumFileStructure::getScoreDuration -- Return the total duration
//    of the score in quarter note units.  Returns zero if no lines in the
//    file, or -1 if there are lines, but no rhythmic analysis has been done.
//

HumNum HumdrumFileStructure::getScoreDuration(void) const {
	if (m_lines.size() == 0) {
		return 0;
	}
	return m_lines.back()->getDurationFromStart();
}



//////////////////////////////
//
// HumdrumFileStructure::tpq -- "Ticks per Quarter-note".  Returns the minimal
//    number of integral time units that divide a quarter note into equal
//    subdivisions.  This value is needed to convert Humdrum data into
//    MIDI file data, MuseData, and MusicXML data.  Also useful for timebase
//    type of operations on the data and describing the durations in terms
//    of integers rather than with fractions.  This function will also
//    consider the implicit durations of non-rhythmic spine data.
//

int HumdrumFileStructure::tpq(void) {
	if (m_ticksperquarternote > 0) {
		return m_ticksperquarternote;
	}
	set<HumNum> durlist = getPositiveLineDurations();
	vector<int> dems;
	for (auto& it : durlist) {
		if (it.getDenominator() > 1) {
			dems.push_back(it.getDenominator());
		}
	}
	int lcm = 1;
	if (dems.size() > 0) {
		lcm = Convert::getLcm(dems);
	}
	m_ticksperquarternote = lcm;
	return m_ticksperquarternote;
}



//////////////////////////////
//
// HumdrumFileStructure::getPositiveLineDurations -- Return a list of all
//    unique token durations in the file.  This function could be expanded
//    to limit the search to a range of lines or to a specific track.
//

set<HumNum> HumdrumFileStructure::getPositiveLineDurations(void) {
	set<HumNum> output;
	for (auto& line : m_lines) {
		if (line->getDuration().isPositive()) {
			output.insert(line->getDuration());
		}
	}
	return output;
}



//////////////////////////////
//
// HumdrumFileStructure::printDurationInfo -- Print the assigned duration
//    of each line in a file.  Useful for debugging.
//

ostream& HumdrumFileStructure::printDurationInfo(ostream& out) {
	for (int i=0; i<getLineCount(); i++) {
		m_lines[i]->printDurationInfo(out) << '\n';
	}
	return out;
}



//////////////////////////////
//
// HumdrumFileStructure::getBarline -- Return the given barline from the file
//   based on the index number.  Negative index accesses from the end of the
//   list.  If the first barline is a pickup measure, then the returned
//   HumdrumLine* will not be an actual barline line.
//

HumdrumLine* HumdrumFileStructure::getBarline(int index) const {
	if (index < 0) {
		index += (int)m_barlines.size();
	}
	if (index < 0) {
		return NULL;
	}
	if (index >= (int)m_barlines.size()) {
		return NULL;
	}
	return m_barlines[index];
}



//////////////////////////////
//
// HumdrumFileStructure::getBarlineCount -- Return the number of barlines in
//   the file.  If there is a pickup beat, then the count includes an imaginary
//   barline before the first pickup (and the start of the file will be returned
//   for barline(0).
//

int HumdrumFileStructure::getBarlineCount(void) const {
	return (int)m_barlines.size();
}



///////////////////////////////
//
// HumdrumFileStructure::getBarlineDuration --  Return the duration from the
//    current barline to the next barline in the data.  For the last barline,
//    the duration will be calculated from the end of the data;  The final
//    will have a duration of 0 if there are not notes after the barline
//    in the data.
//

HumNum HumdrumFileStructure::getBarlineDuration(int index) const {
	if (index < 0) {
		index += (int)m_barlines.size();
	}
	if (index < 0) {
		return 0;
	}
	if (index >= (int)m_barlines.size()) {
		return 0;
	}
	HumNum startdur = m_barlines[index]->getDurationFromStart();
	HumNum enddur;
	if (index + 1 < (int)m_barlines.size() - 1) {
		enddur = m_barlines[index+1]->getDurationFromStart();
	} else {
		enddur = getScoreDuration();
	}
	return enddur - startdur;
}



///////////////////////////////
//
// HumdrumFileStructure::getBarlineDurationFromStart -- Return the duration
//    between the start of the Humdrum file and the given barline.
//

HumNum HumdrumFileStructure::getBarlineDurationFromStart(int index) const {
	if (index < 0) {
		index += (int)m_barlines.size();
	}
	if (index < 0) {
		return 0;
	}
	if (index >= (int)m_barlines.size()) {
		return getScoreDuration();
	}
	return m_barlines[index]->getDurationFromStart();
}



///////////////////////////////
//
// HumdrumFileStructure::getBarlineDurationToEnd -- Return the duration
//    between barline and the end of the HumdrumFileStructure.
//

HumNum HumdrumFileStructure::getBarlineDurationToEnd(int index) const {
	if (index < 0) {
		index += (int)m_barlines.size();
	}
	if (index < 0) {
		return 0;
	}
	if (index >= (int)m_barlines.size()) {
		return getScoreDuration();
	}
	return m_barlines[index]->getDurationToEnd();
}


//////////////////////////////
//
// HumdrumFileStructure::setLineRhythmAnalyzed --
//

void HumdrumFileStructure::setLineRhythmAnalyzed(void) {
	for (int i=0; i<(int)m_lines.size(); i++) {
		m_lines[i]->m_rhythm_analyzed = true;
	}
}


//////////////////////////////
//
// HumdrumFileStructure::analyzeRhythm -- Analyze the rhythmic structure
//     of the data.  Returns false if there was a parse error.
//

bool HumdrumFileStructure::analyzeRhythm(void) {
	setLineRhythmAnalyzed();
	if (getMaxTrack() == 0) {
		return true;
	}
	int startline = getTrackStart(1)->getLineIndex();
	int testline;
	HumNum zero(0);

	int i;
	for (int i=1; i<=getMaxTrack(); i++) {
		if (!getTrackStart(i)->hasRhythm()) {
			// Can't analyze rhythm of spines that do not have rhythm.
			continue;
		}
		testline = getTrackStart(i)->getLineIndex();
		if (testline == startline) {
			if (!assignDurationsToTrack(getTrackStart(i), zero)) {
				return false;
			}
		} else {
			// Spine does not start at beginning of data, so
			// the starting position of the spine has to be
			// determined before continuing.  Search for a token
			// which is on a line with assigned duration, then work
			// outwards from that position.
			continue;
		}
	}

	// Go back and analyze spines that do not start at the
	// beginning of the data stream.
	for (i=1; i<=getMaxTrack(); i++) {
		if (!getTrackStart(i)->hasRhythm()) {
			// Can't analyze rhythm of spines that do not have rhythm.
			continue;
		}
		testline = getTrackStart(i)->getLineIndex();
		if (testline > startline) {
			if (!analyzeRhythmOfFloatingSpine(getTrackStart(i))) { return false; }
		}
	}

	if (!analyzeNullLineRhythms()) { return false; }
	fillInNegativeStartTimes();
	assignLineDurations();
	if (!analyzeMeter()) { return false; }
	if (!analyzeNonNullDataTokens()) { return false; }

	return true;
}



//////////////////////////////
//
// HumdrumFileStructure::analyzeMeter -- Store the times from the last barline
//     to the current line, as well as the time to the next barline.
//     the sum of these two will be the duration of the barline, except
//     for barlines, where the getDurationToBarline() will store the
//     duration of the measure staring at that barline.  To get the
//     beat, you will have to figure out the current time signature.
//

bool HumdrumFileStructure::analyzeMeter(void) {

	m_barlines.resize(0);

	int i;
	HumNum sum = 0;
	bool foundbarline = false;
	for (i=0; i<getLineCount(); i++) {
		m_lines[i]->setDurationFromBarline(sum);
		sum += m_lines[i]->getDuration();
		if (m_lines[i]->isBarline()) {
			foundbarline = true;
			m_barlines.push_back(m_lines[i]);
			sum = 0;
		}
		if (m_lines[i]->isData() && !foundbarline) {
			// pickup measure, so set the first measure to the start of the file.
			m_barlines.push_back(m_lines[0]);
			foundbarline = 1;
		}
	}

	sum = 0;
	for (i=getLineCount()-1; i>=0; i--) {
		sum += m_lines[i]->getDuration();
		m_lines[i]->setDurationToBarline(sum);
		if (m_lines[i]->isBarline()) {
			sum = 0;
		}
	}

	return true;
}



//////////////////////////////
//
// HumdrumFileStructure::analyzeTokenDurations -- Calculate the duration of
//   all tokens in spines which posses duration in a file.
//

bool HumdrumFileStructure::analyzeTokenDurations (void) {
	for (int i=0; i<getLineCount(); i++) {
		if (!m_lines[i]->analyzeTokenDurations(m_parseError)) {
			return isValid();
		}
	}
	return isValid();
}



///////////////////////////////
//
// HumdrumFileStructure::analyzeGlobalParameters -- only allowing layout
//    parameters at the moment.  Global parameters affect the next
//    line which is either a barline, dataline or an interpretation
//    other than a spine manipulator.  Null lines are also not
//    considered.
//

bool HumdrumFileStructure::analyzeGlobalParameters(void) {
	vector<HumdrumLine*> globals;

//	for (int i=0; i<(int)m_lines.size(); i++) {
//		if (m_lines[i]->isCommentGlobal()) {
//			m_lines[i]->setLayoutParameters();
//		}
//	}

	for (int i=0; i<(int)m_lines.size(); i++) {
		if (m_lines[i]->isCommentGlobal() && (m_lines[i]->find("!!LO:") != string::npos)) {
			m_lines[i]->storeGlobalLinkedParameters();
			globals.push_back(m_lines[i]);
			continue;
		}
		if (!m_lines[i]->hasSpines()) {
			continue;
		}
		if (m_lines[i]->isAllNull())  {
			continue;
		}
		if (m_lines[i]->isCommentLocal()) {
			continue;
		}
		if (globals.empty()) {
			continue;
		}

		// Filter manipulators or not?  At the moment allow
		// global parameters to pass through manipulators.
		// if (m_lines[i]->isManipulator()) {
		// 	continue;
		// }

		for (int j=0; j<(int)m_lines[i]->getFieldCount(); j++) {
			for (int k=0; k<(int)globals.size(); k++) {
				m_lines[i]->token(j)->addLinkedParameter(globals[k]->token(0));
			}
		}
		globals.clear();
	}

	return isValid();
}



///////////////////////////////
//
// HumdrumFileStructure::analyzeLocalParameters -- Parses any
//    local comments before a non-null token.
//

bool HumdrumFileStructure::analyzeLocalParameters(void) {
	// analyze backward tokens:

	for (int i=0; i<getStrandCount(); i++) {
		processLocalParametersForStrand(i);
	}

//	for (int i=1; i<=getMaxTrack(); i++) {
//		for (int j=0; j<getTrackEndCount(i); j++) {
//			if (!processLocalParametersForTrack(getTrackEnd(i, j),
//					getTrackEnd(i, j), getTrackStart(i, j))) {
//				return isValid();
//			}
//		}
//	}

	return isValid();
}



//////////////////////////////
//
// HumdrumFileStructure::analyzeDurationsOfNonRhythmicSpines -- Calculate the
//    duration of non-null data token in non-rhythmic spines.
//

bool HumdrumFileStructure::analyzeDurationsOfNonRhythmicSpines(void) {
	// analyze tokens backwards:
	for (int i=1; i<=getMaxTrack(); i++) {
		for (int j=0; j<getTrackEndCount(i); j++) {
			if (getTrackEnd(i, j)->hasRhythm()) {
				continue;
			}
			if (!assignDurationsToNonRhythmicTrack(getTrackEnd(i, j),
					getTrackEnd(i, j))) {
				return isValid();
			}
		}
	}
	return isValid();
}



//////////////////////////////
//
// HumdrumFileStructure::getMinDur -- Return the smallest duration on the
//   line.  If all durations are zero, then return zero; otherwise,
//   return the smallest positive duration.
//

HumNum HumdrumFileStructure::getMinDur(vector<HumNum>& durs,
		vector<HumNum>& durstate) {
	HumNum mindur = 0;
	bool allzero = true;

	for (int i=0; i<(int)durs.size(); i++) {
		if (durs[i].isPositive()) {
			allzero = false;
			if (mindur.isZero()) {
				mindur = durs[i];
			} else if (mindur > durs[i]) {
				mindur = durs[i];
			}
		}
	}
	if (allzero) {
		return mindur;
	}

	for (int i=0; i<(int)durs.size(); i++) {
		if (durstate[i].isPositive() && mindur.isZero()) {
			if (durstate[i].isZero()) {
				// mindur = durstate[i];
			} else if (mindur > durstate[i]) {
				mindur = durstate[i];
			}
		}
	}
	return mindur;
}



//////////////////////////////
//
// HumdrumFileStructure::getTokenDurations -- Extract the duration of rhythmic
//    tokens on the line.
//

bool HumdrumFileStructure::getTokenDurations(vector<HumNum>& durs, int line) {
	durs.resize(0);
	for (int i=0; i<m_lines[line]->getTokenCount(); i++) {
		HumNum dur = m_lines[line]->token(i)->getDuration();
		durs.push_back(dur);
	}
	if (!cleanDurs(durs, line)) {
		return isValid();
	}
	return isValid();
}



//////////////////////////////
//
// HumdrumFileStructure::cleanDurs -- Check if there are grace note and regular
//    notes on a line (not allowed).  Leaves negative durations which
//    indicate undefined durations (needed for keeping track of null
//    tokens in rhythmic spines.
//

bool HumdrumFileStructure::cleanDurs(vector<HumNum>& durs, int line) {
	bool zero     = false;
	bool positive = false;
	for (int i=0; i<(int)durs.size(); i++) {
		if      (durs[i].isPositive()) { positive = true; }
		else if (durs[i].isZero())     { zero     = true; }
	}
	if (zero && positive) {
		stringstream err;
		err << "Error on line " << (line+1) << " grace note and "
		    << " regular note cannot occur on same line." << endl;
		err << "Line: " << *m_lines[line] << endl;
		return setParseError(err);
	}
	return isValid();
}



//////////////////////////////
//
// HumdrumFileStructure::decrementDurStates -- Subtract the line duration from
//   the current line of running durations.  If any duration is less
//   than zero, then a rhythm error exists in the data.
//

bool HumdrumFileStructure::decrementDurStates(vector<HumNum>& durs,
		HumNum linedur, int line) {
	if (linedur.isZero()) {
		return isValid();
	}
	for (int i=0; i<(int)durs.size(); i++) {
		if (!m_lines[line]->token(i)->hasRhythm()) {
			continue;
		}
		durs[i] -= linedur;
		if (durs[i].isNegative()) {
			stringstream err;
			err << "Error: rhythmic error on line " << (line+1)
			    << " field index " << i << endl;
			err << "Duration state is: " << durs[i] << endl;
			return setParseError(err);
		}
	}
	return isValid();
}



//////////////////////////////
//
// HumdrumFileStructure::assignDurationsToTrack -- Assign duration from starts
//    for each rhythmic spine in the file.  Analysis is done recursively, one
//    sub-spine at a time.  Duplicate analyses are prevented by the state
//    variable in the HumdrumToken (currently called rhycheck because it is only
//    used in this function).  After the durationFromStarts have been assigned
//    for the rhythmic analysis of non-data tokens and non-rhythmic spines is
//    done elsewhere.
//

bool HumdrumFileStructure::assignDurationsToTrack(HTp starttoken,
		HumNum startdur) {
	if (!starttoken->hasRhythm()) {
		return isValid();
	}
	int state = starttoken->getState();
	if (!prepareDurations(starttoken, state, startdur)) {
		return isValid();
	}
	return isValid();
}



//////////////////////////////
//
// HumdrumFileStructure::prepareDurations -- Helper function for
//     HumdrumFileStructure::assignDurationsToTrack() which does all of the
//     work for assigning durationFromStart values.
//

bool HumdrumFileStructure::prepareDurations(HTp token, int state,
		HumNum startdur) {
	if (state != token->getState()) {
		return isValid();
	}

	HumNum dursum = startdur;
	token->incrementState();

	if (!setLineDurationFromStart(token, dursum)) { return isValid(); }
	if (token->getDuration().isPositive()) {
		dursum += token->getDuration();
	}
	int tcount = token->getNextTokenCount();

	vector<HTp> reservoir;
	vector<HumNum> startdurs;

	// Assign line durationFromStarts for primary track first.
	while (tcount > 0) {
		for (int t=1; t<tcount; t++) {
			reservoir.push_back(token->getNextToken(t));
			startdurs.push_back(dursum);
		}
		token = token->getNextToken(0);
		if (state != token->getState()) {
			break;
		}
		token->incrementState();
		if (!setLineDurationFromStart(token, dursum)) { return isValid(); }
		if (token->getDuration().isPositive()) {
			dursum += token->getDuration();
		}
		tcount = token->getNextTokenCount();
	}

	if ((tcount == 0) && (token->isTerminateInterpretation())) {
		if (!setLineDurationFromStart(token, dursum)) { return isValid(); }
	}

	// Process secondary tracks next:
	int newstate = state;

	for (int i=(int)reservoir.size()-1; i>=0; i--) {
		prepareDurations(reservoir[i], newstate, startdurs[i]);
	}

	return isValid();
}



//////////////////////////////
//
// HumdrumFileStructure::setLineDurationFromStart -- Set the duration of
//      a line based on the analysis of tokens in the spine.
//

bool HumdrumFileStructure::setLineDurationFromStart(HTp token,
		HumNum dursum) {
	if ((!token->isTerminateInterpretation()) &&
			token->getDuration().isNegative()) {
		// undefined rhythm, so don't assign line duration information:
		return isValid();
	}
	HumdrumLine* line = token->getOwner();
	if (line->getDurationFromStart().isNegative()) {
		line->setDurationFromStart(dursum);
	} else if (line->getDurationFromStart() != dursum) {
		stringstream err;
		err << "Error: Inconsistent rhythm analysis occurring near line "
		    << token->getLineNumber() << endl;
		err << "Expected durationFromStart to be: " << dursum
		    << " but found it to be " << line->getDurationFromStart() << endl;
		err << "Line: " << *line << endl;
		return setParseError(err);
	}

	return isValid();
}



//////////////////////////////
//
// HumdrumFileStructure::analyzeRhythmOfFloatingSpine --  This analysis
//    function is used to analyze the rhythm of spines which do not start at
//    the beginning of the data.  The function searches for the first line
//    which has an assigned durationFromStart value, and then uses that
//    as the basis for assigning the initial durationFromStart position
//    for the spine.
//

bool HumdrumFileStructure::analyzeRhythmOfFloatingSpine(
		HTp spinestart) {
	HumNum dursum = 0;
	HumNum founddur = 0;
	HTp token = spinestart;
	int tcount = token->getNextTokenCount();

	// Find a known durationFromStart for a line in the Humdrum file, then
	// use that to calculate the starting duration of the floating spine.
	if (token->getDurationFromStart().isNonNegative()) {
		founddur = token->getLine()->getDurationFromStart();
	} else {
		tcount = token->getNextTokenCount();
		while (tcount > 0) {
			if (token->getDurationFromStart().isNonNegative()) {
				founddur = token->getLine()->getDurationFromStart();
				break;
			}
			if (token->getDuration().isPositive()) {
				dursum += token->getDuration();
			}
			token = token->getNextToken(0);
		}
	}

	if (founddur.isZero()) {
		return setParseError("Error cannot link floating spine to score.");
	}

	if (!assignDurationsToTrack(spinestart, founddur - dursum)) {
		return isValid();
	}

	return isValid();
}



//////////////////////////////
//
// HumdrumFileStructure::analyzeNullLineRhythms -- When a series of null-token
//    data line occur between two data lines possessing a start duration,
//    then split the duration between those two lines amongst the null-token
//    lines.  For example if a data line starts at time 15, and there is one
//    null-token line before another data line at time 16, then the null-token
//    line will be assigned to the position 15.5 in the score.
//

bool HumdrumFileStructure::analyzeNullLineRhythms(void) {
	vector<HumdrumLine*> nulllines;
	HumdrumLine* previous = NULL;
	HumdrumLine* next = NULL;
	HumNum dur;
	HumNum startdur;
	HumNum enddur;
	int i, j;
	for (i=0; i<(int)m_lines.size(); i++) {
		if (!m_lines[i]->hasSpines()) {
			continue;
		}
		if (m_lines[i]->isAllRhythmicNull()) {
			if (m_lines[i]->isData()) {
				nulllines.push_back(m_lines[i]);
			}
			continue;
		}
		dur = m_lines[i]->getDurationFromStart();
		if (dur.isNegative()) {
			if (m_lines[i]->isData()) {
				stringstream err;
				err << "Error: found an unexpected negative duration on line "
			       << m_lines[i]->getDurationFromStart()<< endl;
				err << "Line: " << *m_lines[i] << endl;
				return setParseError(err);
			} else {
				continue;
			}
		}
		next = m_lines[i];
		if (previous == NULL) {
			previous = next;
			nulllines.resize(0);
			continue;
		}
		startdur = previous->getDurationFromStart();
		enddur   = next ->getDurationFromStart();
		HumNum gapdur = enddur - startdur;
		HumNum nulldur = gapdur / ((int)nulllines.size() + 1);
		for (j=0; j<(int)nulllines.size(); j++) {
			nulllines[j]->setDurationFromStart(startdur + (nulldur * (j+1)));
		}
		previous = next;
		nulllines.resize(0);
	}
	return isValid();
}



//////////////////////////////
//
// HumdrumFileStructure::fillInNegativeStartTimes -- Negative line durations
//    after the initial rhythmAnalysis mean that the lines are not data line.
//    Duplicate the duration of the next non-negative duration for all negative
//    durations.
//

void HumdrumFileStructure::fillInNegativeStartTimes(void) {
	int i;
	HumNum lastdur = -1;
	HumNum dur;
	for (i=(int)m_lines.size()-1; i>=0; i--) {
		dur = m_lines[i]->getDurationFromStart();
		if (dur.isNegative() && lastdur.isNonNegative()) {
			m_lines[i]->setDurationFromStart(lastdur);
		}
		if (dur.isNonNegative()) {
			lastdur = dur;
			continue;
		}
	}

	// fill in start times for ending comments
	for (i=0; i<(int)m_lines.size(); i++) {
		dur = m_lines[i]->getDurationFromStart();
		if (dur.isNonNegative()) {
			lastdur = dur;
		} else {
			m_lines[i]->setDurationFromStart(lastdur);
		}
	}
}



//////////////////////////////
//
// HumdrumFileStructure::assignLineDurations --  Calculate the duration of lines
//   based on the durationFromStart of the current line and the next line.
//

void HumdrumFileStructure::assignLineDurations(void) {
	HumNum startdur;
	HumNum enddur;
	HumNum dur;
	for (int i=0; i<(int)m_lines.size()-1; i++) {
		startdur = m_lines[i]->getDurationFromStart();
		enddur = m_lines[i+1]->getDurationFromStart();
		dur = enddur - startdur;
		m_lines[i]->setDuration(dur);
	}
	if (m_lines.size() > 0) {
		m_lines.back()->setDuration(0);
	}
}



//////////////////////////////
//
// HumdrumFileStructure::assignDurationsToNonRhythmicTrack --  After the basic
//   rhythmAnalysis has been done, go back and assign durations to non-rhythmic
//   spine tokens based on the lineFromStart values of the lines that they
//   occur on as well as the distance in the file to the next non-null token for
//   that spine.
//

bool HumdrumFileStructure::assignDurationsToNonRhythmicTrack(
		HTp endtoken, HTp current) {

	string spineinfo = endtoken->getSpineInfo();
	HTp token = endtoken;

	while (token) {
		if (token->getSpineInfo() != spineinfo) {
			if (token->getSpineInfo().find("b") != std::string::npos) {
				break;
			}
			if (spineinfo.find("b") != std::string::npos) {
				break;
			}
		}
		int tcount = token->getPreviousTokenCount();
		if (tcount == 0) {
			break;
		}
		if (tcount > 1) {
			for (int i=1; i<tcount; i++) {
				HTp ptok = token->getPreviousToken(i);
				if (!assignDurationsToNonRhythmicTrack(ptok, current)) {
					return isValid();
				}
			}
		}
		if (token->isData()) {
			if (!token->isNull()) {
				token->setDuration(current->getDurationFromStart() -
					token->getDurationFromStart());
				current = token;
			}
		}
		token = token->getPreviousToken(0);
	}

	return isValid();
}



//////////////////////////////
//
// HumdrumFileStructure::processLocalParametersForStrand --
//

void HumdrumFileStructure::processLocalParametersForStrand(int index) {
	HTp sstart = getStrandStart(index);
	HTp send = getStrandEnd(index);
	HTp tok = send;
	HTp dtok = NULL;
	while (tok) {
		if (tok->isData()) {
			dtok = tok;
		} else if (tok->isBarline()) {
			// layout parameters allowed for barlines
			dtok = tok;
		} else if (tok->isInterpretation() && (*tok != "*")) {
			// layout parameters allowed for non-null interpretations
			dtok = tok;
		} else if (tok->isCommentLocal()) {
			if (tok->find("!LO:") == 0) {
				tok->storeLinkedParameters();
				if (dtok) {
					dtok->addLinkedParameter(tok);
				}
			}
		}
		if (tok == sstart) {
			break;
		}
		tok = tok->getPreviousToken();
	}
}




//////////////////////////////
//
// HumdrumFileStructure::processLocalParametersForTrack --  Search for
//   local parameters backwards in each spine and fill in the HumHash
//   for the token to which the parameter is to be applied.
//
// No longer used.
//

bool HumdrumFileStructure::processLocalParametersForTrack(
		HTp starttok, HTp current) {

	HTp token = starttok;
	int tcount = token->getPreviousTokenCount();

	while (tcount > 0) {
		for (int i=1; i<tcount; i++) {
			if (!processLocalParametersForTrack(
					token->getPreviousToken(i), current)) {
				return isValid();
			}
		}
		HTp prevtoken = token->getPreviousToken();
		if (prevtoken->isSplitInterpretation()) {
			if (token != prevtoken->m_nextTokens[0]) {
				// terminate if not most primary subspine
				return true;
			}
		} else if (!(token->isNull() & token->isManipulator())) {
			if (token->isCommentLocal()) {
				checkForLocalParameters(token, current);
			} else {
				current = token;
			}
		}

		// Follow previous data token 0 since 1 and higher are handled above.
		token = token->getPreviousToken(0);
		tcount = token->getPreviousTokenCount();
	}

	return isValid();
}



//////////////////////////////
//
// HumdrumFileStructure::checkForLocalParameters -- Helper function for
//     HumdrumFileStructure::processLocalParametersForTrack.  Only allowing
//     layout parameters currently.
//

void HumdrumFileStructure::checkForLocalParameters(HTp token,
		HTp current) {
	if (token->size() < 1) {
		return;
	}
	int loc1 = (int)token->find(":");
	if (loc1 == (int)string::npos) {
		return;
	}
	int loc2 = (int)token->substr(loc1).find(":");
	if (loc2 == (int)string::npos) {
		return;
	}
	loc2 += loc1 + 1;
	int sloc = (int)token->find(" ");
	if (sloc != (int)string::npos) {
		if ((sloc < loc1) || (sloc < loc2)) {
			return;
		}
	}
	sloc = (int)token->find("\t");
	if (sloc != (int)string::npos) {
		if ((sloc < loc1) || (sloc < loc2)) {
			return;
		}
	}
	// Looks like a parameter so parse the comment:
	current->setParameters(token);
}



//////////////////////////////
//
// HumdrumFileStructure::analyzeStrands -- Analyze spine strands.
//

bool HumdrumFileStructure::analyzeStrands(void) {
	m_strands_analyzed = true;
	int spines = getSpineCount();
	m_strand1d.resize(0);
	m_strand2d.resize(0);
	int i, j;
	for (i=0; i<spines; i++) {
		HTp tok = getSpineStart(i);
		m_strand2d.resize(m_strand2d.size()+1);
		analyzeSpineStrands(m_strand2d.back(), tok);
	}

	for (i=0; i<(int)m_strand2d.size(); i++) {
		std::sort(m_strand2d[i].begin(), m_strand2d[i].end(),
				sortTokenPairsByLineIndex);
		for (j=0; j<(int)m_strand2d[i].size(); j++) {
			m_strand1d.push_back(m_strand2d[i][j]);
		}
	}

	assignStrandsToTokens();

	resolveNullTokens();

	return isValid();
}



///////////////////////////////
//
// HumdrumFileStructure::resolveNullTokens --
//

void HumdrumFileStructure::resolveNullTokens(void) {
	if (m_nulls_analyzed) {
		return;
	}
	m_nulls_analyzed = true;
	if (!areStrandsAnalyzed()) {
		analyzeStrands();
	}

	HTp token;
	HTp data = NULL;
	HTp strandend;
	for (int s=0; s<(int)m_strand1d.size(); s++) {
		token = getStrandStart(s);
		strandend = getStrandEnd(s);
		while (token != strandend) {
			if (!token->isData()) {
				token = token->getNextToken();
				continue;
			}
			if (data == NULL) {
				data = token;
				token->setNullResolution(data);
				token = token->getNextToken();
				continue;
			}
			if (token->isNull()) {
				token->setNullResolution(data);
			} else {
				data = token;
			}
			token = token->getNextToken();
		}
	}
}



//////////////////////////////
//
// HumdrumFileStructure::assignStrandsToTokens -- Store the 1D strand
//    index number for each token in the file.  Global tokens will have
//    strand index set to -1.
//

void HumdrumFileStructure::assignStrandsToTokens(void) {
	HTp tok;
	for (int i=0; i<(int)m_strand1d.size(); i++) {
		tok = m_strand1d[i].first;
		while (tok != NULL) {
			tok->setStrandIndex(i);
			tok = tok->getNextToken();
		}
	}
}



//////////////////////////////
//
// HumdrumFileStructure::analyzeSpineStrands -- Fill in the list of
//   strands in a single spine.
//

void HumdrumFileStructure::analyzeSpineStrands(vector<TokenPair>& ends,
		HTp starttok) {

	ends.resize(ends.size()+1);
	int index = (int)ends.size()-1;
	ends[index].first = starttok;
	HTp tok = starttok;
	while (tok != NULL) {
		if ((tok->getSubtrack() > 1) && (tok->isMerge())) {
			// check to the left: if the left primary/sub spine also has
			// a *v, then this is the end of this strand; otherwise, the
			// strand continues.
			if (tok->getPreviousFieldToken()->isMerge()) {
				ends[index].last = tok;
				return;
			} else {
				tok = tok->getNextToken();
				continue;
			}
		}
		if (tok->isTerminator()) {
			ends[index].last = tok;
			return;
		}
		if (tok->getNextTokenCount() > 1) {
			// should only be 2, but allow for generalizing in the future.
			for (int j=1; j<tok->getNextTokenCount(); j++) {
				analyzeSpineStrands(ends, tok->getNextToken(j));
			}
		}
		tok = tok->getNextToken();
	}

	cerr << "Should not get here in analyzeSpineStrands()\n";
}


//////////////////////////////
//
// HumdrumFileStructure::getStrandCount --
//

int HumdrumFileStructure::getStrandCount(void) {
	if (!areStrandsAnalyzed()) {
		analyzeStrands();
	}
	return (int)m_strand1d.size();
}


int HumdrumFileStructure::getStrandCount(int spineindex) {
	if (!areStrandsAnalyzed()) {
		analyzeStrands();
	}
	if (spineindex < 0) {
		return 0;
	}
	if (spineindex >= (int)m_strand2d.size()) {
		return 0;
	}
	return (int)m_strand2d[spineindex].size();
}



//////////////////////////////
//
// HumdrumFileStructure::getStrandStart -- Return the first token
//    in the a strand.
//

HTp HumdrumFileStructure::getStrandStart(int index) {
	if (!areStrandsAnalyzed()) {
		analyzeStrands();
	}
	return m_strand1d[index].first;
}


HTp HumdrumFileStructure::getStrandEnd(int index) {
	if (!areStrandsAnalyzed()) {
		analyzeStrands();
	}
	return m_strand1d[index].last;
}


HTp HumdrumFileStructure::getStrandStart(int sindex,
		int index) {
	if (!areStrandsAnalyzed()) {
		analyzeStrands();
	}
	return m_strand2d[sindex][index].first;
}


HTp HumdrumFileStructure::getStrandEnd(int sindex, int index) {
	if (!areStrandsAnalyzed()) {
		analyzeStrands();
	}
	return m_strand2d[sindex][index].last;
}



//////////////////////////////
//
// HumdrumFileStructure::hasFilters -- Returns true if has any
//    reference records starting with "!!!filter:" or "!!!!filter:".
//

bool HumdrumFileStructure::hasFilters(void) {
	HumdrumFileBase& infile = *this;
	vector<HumdrumLine*> refs  = infile.getGlobalReferenceRecords();
	for (int i=0; i<(int)refs.size(); i++) {
		if (refs[i]->getGlobalReferenceKey() == "filter") {
			return true;
		}
	}
	return false;
}



//////////////////////////////
//
// HumdrumFileStructure::hasGlobalFilters -- Returns true if has any
//    reference records starting with "!!!filter:".
//

bool HumdrumFileStructure::hasGlobalFilters(void) {
	HumdrumFileBase& infile = *this;
	for (int i=0; i<infile.getLineCount(); i++) {
		if (!infile[i].isComment()) {
			continue;
		}
		HTp token = infile.token(i, 0);
		if (token->compare(0, 10, "!!!filter:") == 0) {
			return true;
		}
	}
	return false;
}



//////////////////////////////
//
// HumdrumFileStructure::hasUniversalFilters -- Returns true if has any
//    reference records starting with "!!!!filter:".
//

bool HumdrumFileStructure::hasUniversalFilters(void) {
	HumdrumFileBase& infile = *this;
	vector<HumdrumLine*> refs  = infile.getUniversalReferenceRecords();
	for (int i=0; i<(int)refs.size(); i++) {
		if (refs[i]->getUniversalReferenceKey() == "filter") {
			return true;
		}
	}
	return false;
}



//////////////////////////////
//
// HumdrumFileStructure::analyzeSignifiers --
//

void HumdrumFileStructure::analyzeSignifiers(void) {
	HumdrumFileStructure& infile = *this;
	for (int i=0; i<getLineCount(); i++) {
		if (!infile[i].isSignifier()) {
			continue;
		}
		m_signifiers.addSignifier(infile[i].getText());
	}
}



//////////////////////////////
//
// HumdrumFileStructure::getKernLinkSignifier -- used for linking two
//     non-standard slur/tie ends together.
//

std::string HumdrumFileStructure::getKernLinkSignifier(void) {
	return m_signifiers.getKernLinkSignifier();
}



//////////////////////////////
//
// HumdrumFileStructure::getKernAboveSignifier -- used to place things
//     "above" (note on staff above, slurs/ties with an "above" orientation,
//     etc.
//

std::string HumdrumFileStructure::getKernAboveSignifier(void) {
	return m_signifiers.getKernAboveSignifier();
}



//////////////////////////////
//
// HumdrumFileStructure::getKernBelowSignifier -- used to place things
//     "below" (note on staff above, slurs/ties with an "below" orientation,
//     etc.
//

std::string HumdrumFileStructure::getKernBelowSignifier(void) {
	return m_signifiers.getKernBelowSignifier();
}



// END_MERGE

} // end namespace hum




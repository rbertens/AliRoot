//____________________________________________________________________
//
// $Id$
//
// Draw hits in the specialised FMD event display 
//
/** Pattern hits 
    @ingroup FMD_script
 */
void
PatternHits()
{
  // AliCDBManager* cdb = AliCDBManager::Instance();
  // cdb->SetDefaultStorage("local://cdb");
  gSystem->Load("libFMDutil");
  AliFMDPattern* d = new AliFMDPattern;
  d->AddLoad(AliFMDInput::kHits);
  d->SetName("hit");
  d->SetTitle("Hits");
  // d->AddLoad(AliFMDInput::kKinematics);
  d->Run();
}

//____________________________________________________________________
//
// EOF
//
